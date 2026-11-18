/*
 * HaLow 802.11s mesh example.
 *
 * Roles:
 *   HALOW_MESH_ROLE_GATEWAY  (static IP + DHCP server for mesh points)
 *   HALOW_MESH_ROLE_POINT    (mesh point, DHCP client or static IP)
 *
 * DHCP:
 *   HALOW_POINT_USE_DHCP=1   (point DHCP, default)
 *   HALOW_POINT_USE_DHCP=0   (point static IP)
 * 
 * WHITELIST: 
 *   The whitelist is intended for path testing and for pinning a fixed topology.
 *   MESH_PEER_WHITELIST_ENABLE=1   (WHITELIST enable, only can connect MESH_PEER_WHITELIST_0-MESH_PEER_WHITELIST_3)
 *   MESH_PEER_WHITELIST_ENABLE=0   (WHITELIST disable, can connect device with same pesh paras)
 *
 *   The whitelist contains peer mesh MACs (the "Mesh MAC" printed by each node).
 *   All nodes must use the same region, mesh ID, password, channel and bandwidth.
 */
#include <HaLow.h>

/* ------------------------ Configuration options ------------------------ */

#define HALOW_MESH_ROLE_GATEWAY 1
#define HALOW_MESH_ROLE_POINT   2

#ifndef HALOW_MESH_ROLE
#define HALOW_MESH_ROLE HALOW_MESH_ROLE_GATEWAY
#endif

#ifndef HALOW_REGION
#define HALOW_REGION "US"
#endif
#ifndef MESH_ID
#define MESH_ID "HaLow-Mesh"
#endif
#ifndef MESH_PASSWORD
#define MESH_PASSWORD "heltec.org"
#endif

// Please read REGION_SW_CHANNEL_REF.h to obtain the relationship between region, bandwidth,  channel and freq.
#ifndef MESH_CHANNEL
#define MESH_CHANNEL 28
#endif
#ifndef MESH_BANDWIDTH
#define MESH_BANDWIDTH 8
#endif

/* Optional mesh peer whitelist. */
#ifndef MESH_PEER_WHITELIST_ENABLE
#define MESH_PEER_WHITELIST_ENABLE 0
#endif
#ifndef MESH_PEER_WHITELIST_0
#define MESH_PEER_WHITELIST_0 "0C:BF:74:67:6A:5C"
#endif
#ifndef MESH_PEER_WHITELIST_1
#define MESH_PEER_WHITELIST_1 "78:72:64:E5:81:A0"
#endif
#ifndef MESH_PEER_WHITELIST_2
#define MESH_PEER_WHITELIST_2 ""
#endif
#ifndef MESH_PEER_WHITELIST_3
#define MESH_PEER_WHITELIST_3 ""
#endif

/* Set to 0 to disable periodic mesh peer logging. */
#ifndef MESH_PEER_PRINT_INTERVAL_MS
#define MESH_PEER_PRINT_INTERVAL_MS 10000
#endif

/* Gateway network configuration. */
IPAddress gateway_ip(192, 168, 1, 1);
IPAddress netmask(255, 255, 255, 0);
IPAddress dns_primary(8, 8, 8, 8);
IPAddress dns_secondary(8, 8, 4, 4);

/* DHCP pool handed out by the gateway. */
IPAddress dhcp_lease_start(192, 168, 1, 100);
IPAddress dhcp_lease_end(192, 168, 1, 150);

/* Point configuration. */
#ifndef HALOW_POINT_USE_DHCP
#define HALOW_POINT_USE_DHCP 1
#endif
IPAddress point_static_ip(192, 168, 1, 10);

/* ------------------------ End of configuration options ------------------------ */

static const char *mesh_role_name(void)
{
#if HALOW_MESH_ROLE == HALOW_MESH_ROLE_GATEWAY
    return "gateway";
#elif HALOW_MESH_ROLE == HALOW_MESH_ROLE_POINT
    return "point";
#else
    return "unknown";
#endif
}

static const char *mesh_security_name(void)
{
    return MESH_PASSWORD[0] != '\0' ? "SAE" : "OPEN";
}

#if MESH_PEER_WHITELIST_ENABLE
static bool mesh_whitelist_parse_mac(const char *text, uint8_t mac[MMWLAN_MAC_ADDR_LEN])
{
    unsigned value[MMWLAN_MAC_ADDR_LEN];

    if (text == NULL ||
        sscanf(text, "%x:%x:%x:%x:%x:%x",
               &value[0], &value[1], &value[2],
               &value[3], &value[4], &value[5]) != MMWLAN_MAC_ADDR_LEN)
    {
        return false;
    }

    bool all_zero = true;
    for (size_t i = 0; i < MMWLAN_MAC_ADDR_LEN; i++)
    {
        if (value[i] > 0xFF)
        {
            return false;
        }
        mac[i] = (uint8_t)value[i];
        if (mac[i] != 0)
        {
            all_zero = false;
        }
    }

    return !all_zero && !(mac[0] & 0x01);
}
#endif

static bool configure_mesh_whitelist(void)
{
#if MESH_PEER_WHITELIST_ENABLE
    static const char *const configured[] = {
        MESH_PEER_WHITELIST_0,
        MESH_PEER_WHITELIST_1,
        MESH_PEER_WHITELIST_2,
        MESH_PEER_WHITELIST_3,
    };
    uint8_t peers[MMWLAN_MESH_PEER_WHITELIST_MAX][MMWLAN_MAC_ADDR_LEN];
    size_t peer_count = 0;

    for (size_t i = 0; i < MMWLAN_MESH_PEER_WHITELIST_MAX; i++)
    {
        if (configured[i][0] == '\0')
        {
            continue;
        }
        if (!mesh_whitelist_parse_mac(configured[i], peers[peer_count]))
        {
            Serial.printf("[MESH ACL] invalid whitelist MAC: %s\r\n", configured[i]);
            return false;
        }
        peer_count++;
    }

    if (peer_count == 0)
    {
        Serial.println("[MESH ACL] whitelist enabled but no MAC configured");
        return false;
    }

    if (!HaLow.meshSetPeerWhitelist(
            (const uint8_t (*)[MMWLAN_MAC_ADDR_LEN])peers, peer_count))
    {
        Serial.println("[MESH ACL] failed to set whitelist");
        return false;
    }

    Serial.printf("[MESH ACL] whitelist=%u peer(s)\r\n", (unsigned)peer_count);
    for (size_t i = 0; i < peer_count; i++)
    {
        Serial.printf("  %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                      peers[i][0], peers[i][1], peers[i][2],
                      peers[i][3], peers[i][4], peers[i][5]);
    }
    return true;
#else
    Serial.println("[MESH ACL] whitelist disabled");
    return true;
#endif
}

static void print_ip(void)
{
    Serial.printf("IP      : %s\r\n", HaLow.localIP().toString().c_str());
    Serial.printf("Netmask : %s\r\n", HaLow.subnetMask().toString().c_str());
    Serial.printf("Gateway : %s\r\n", HaLow.gatewayIP().toString().c_str());
    Serial.printf("DNS     : %s\r\n", HaLow.dnsIP().toString().c_str());
    Serial.printf("Mesh MAC: %s\r\n", HaLow.macAddress().c_str());
}

static void print_mesh_peers(void)
{
    struct mmwlan_mesh_link_stats links[MMWLAN_MESH_LINK_STATS_MAX];
    size_t count = HaLow.meshGetLinkStats(links, MMWLAN_MESH_LINK_STATS_MAX);
    bool any = false;

    Serial.println("[MESH] peers:");
    for (size_t i = 0; i < count; i++)
    {
        const struct mmwlan_mesh_link_stats *link = &links[i];
        bool zero = true;
        for (size_t j = 0; j < 6; j++)
        {
            if (link->peer[j] != 0)
            {
                zero = false;
                break;
            }
        }
        if (zero)
        {
            continue;
        }

        struct mmipal_sta_rssi signal;
        char noise[8] = "n/a";
        if (mmipal_get_sta_rssi(link->peer, &signal) == MMIPAL_SUCCESS &&
            signal.noise_dbm > -128 && signal.noise_dbm < 0)
        {
            snprintf(noise, sizeof(noise), "%d", signal.noise_dbm);
        }

        any = true;
        Serial.printf("  %02X:%02X:%02X:%02X:%02X:%02X rssi=%d noise=%s rx=%lu tx_ok=%lu tx_fail=%lu\r\n",
                      link->peer[0], link->peer[1], link->peer[2],
                      link->peer[3], link->peer[4], link->peer[5],
                      link->rssi_avg, noise,
                      (unsigned long)link->rx_frames,
                      (unsigned long)link->tx_ok,
                      (unsigned long)link->tx_fail);
    }

    if (!any)
    {
        Serial.println("  (none yet)");
    }
}

#if HALOW_MESH_ROLE == HALOW_MESH_ROLE_GATEWAY
static bool start_gateway_dhcp_server(void)
{
    if (HaLow.isDhcpServerEnabled())
    {
        return true;
    }

    if (!HaLow.enableDhcpServer(dhcp_lease_start, dhcp_lease_end, dns_primary))
    {
        return false;
    }

    Serial.printf("DHCP server started, lease %s - %s\r\n",
                  dhcp_lease_start.toString().c_str(),
                  dhcp_lease_end.toString().c_str());
    return true;
}
#endif

static void print_help(void)
{
    Serial.println();
    Serial.println("Mesh configuration:");
    Serial.printf("  Region    : %s\r\n", HALOW_REGION);
    Serial.printf("  Mesh ID   : %s\r\n", MESH_ID);
    Serial.printf("  Password  : %s (%s)\r\n",
                  MESH_PASSWORD[0] ? MESH_PASSWORD : "<none>",
                  mesh_security_name());
    Serial.printf("  Channel   : %u\r\n", MESH_CHANNEL);
    Serial.printf("  Bandwidth : %u MHz\r\n", MESH_BANDWIDTH);
    Serial.printf("  Role      : %s\r\n", mesh_role_name());
    Serial.printf("  Max peers : %d\r\n", HALOW_MESH_MAX_PEERS);
#if MESH_PEER_WHITELIST_ENABLE
    Serial.println("  Whitelist : enabled");
#else
    Serial.println("  Whitelist : disabled");
#endif

#if HALOW_MESH_ROLE == HALOW_MESH_ROLE_GATEWAY
    Serial.printf("  Gateway IP: %s/%s\r\n",
                  gateway_ip.toString().c_str(), netmask.toString().c_str());
    Serial.printf("  DHCP pool : %s - %s\r\n",
                  dhcp_lease_start.toString().c_str(),
                  dhcp_lease_end.toString().c_str());
#else
#if HALOW_POINT_USE_DHCP
    Serial.println("  IP mode   : DHCP");
#else
    Serial.printf("  Static IP : %s/%s\r\n",
                  point_static_ip.toString().c_str(), netmask.toString().c_str());
    Serial.printf("  Gateway   : %s\r\n", gateway_ip.toString().c_str());
#endif
#endif

    Serial.println();
    Serial.println("All nodes must use the same mesh ID, password, channel and bandwidth.");
}

static void HaLowEvent(HaLowEvent_t event)
{
    Serial.printf("[HaLow-event] %s (%d)\r\n", HaLow.eventName(event), event);
}

void setup()
{
    Serial.begin(115200);
    delay(100);

    HaLow.onEvent(HaLowEvent);

    Serial.printf("\r\nHaLow Mesh example, role=%s\r\n", mesh_role_name());

#ifdef HT_RC3268
    pinMode(HALOW_LDO_CTRL, OUTPUT);
    digitalWrite(HALOW_LDO_CTRL, HALOW_LDO_ENABLE);
#endif

#if HALOW_MESH_ROLE == HALOW_MESH_ROLE_GATEWAY
    if (!HaLow.config(gateway_ip, gateway_ip, netmask, dns_primary, dns_secondary))
    {
        Serial.println("Failed to configure gateway static IP");
    }
#elif HALOW_MESH_ROLE == HALOW_MESH_ROLE_POINT
#if !HALOW_POINT_USE_DHCP
    if (!HaLow.config(point_static_ip, gateway_ip, netmask, dns_primary, dns_secondary))
    {
        Serial.println("Failed to configure point static IP");
    }
#endif
#endif

    if (!configure_mesh_whitelist())
    {
        Serial.println("Mesh whitelist configuration failed");
        return;
    }

    if (!HaLow.meshBegin(MESH_ID, MESH_PASSWORD, MESH_CHANNEL, MESH_BANDWIDTH,
                         HALOW_REGION))
    {
        Serial.println("Failed to start mesh");
        return;
    }
    Serial.println("Mesh mode started");

#if HALOW_MESH_ROLE == HALOW_MESH_ROLE_GATEWAY
    if (!start_gateway_dhcp_server())
    {
        Serial.println("DHCP server not ready yet, will retry after link up");
    }
#endif

    Serial.print("Waiting for mesh link");
    uint32_t link_start = millis();
    while (HaLow.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        if (millis() - link_start > 60000)
        {
            Serial.println();
            Serial.println("Mesh link timeout");
            return;
        }
    }
    Serial.println();
    Serial.println("Mesh link up");

#if HALOW_MESH_ROLE == HALOW_MESH_ROLE_GATEWAY
    if (!start_gateway_dhcp_server())
    {
        Serial.println("Failed to start DHCP server");
    }
#endif

    print_ip();
    print_help();
}

void loop()
{
#if MESH_PEER_PRINT_INTERVAL_MS > 0
    static uint32_t last_peer_print = 0;

    if (millis() - last_peer_print >= MESH_PEER_PRINT_INTERVAL_MS)
    {
        last_peer_print = millis();
        print_mesh_peers();
    }
#endif

    delay(1000);
}
