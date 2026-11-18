/*
 * ---------------------------------------------------------------------------
 * 1. Radio role — HALOW_ROLE
 * ---------------------------------------------------------------------------
 *   HALOW_ROLE_STA           (0, default) ESP32 joins an existing HaLow AP
 *   HALOW_ROLE_AP            (1)          ESP32 acts as a HaLow soft-AP
 *   HALOW_ROLE_MESH_GATEWAY  (2)          ESP32 is an 802.11s mesh gateway
 *   HALOW_ROLE_MESH_POINT    (3)          ESP32 joins as an 802.11s mesh point

 * ---------------------------------------------------------------------------
 * 2. IP address mode — STA and mesh point only
 * ---------------------------------------------------------------------------
 *   HALOW_USE_DHCP=1        (default) obtain the address over DHCP
 *   HALOW_USE_DHCP=0        use the static address below (192.168.1.10)
 *
 * ---------------------------------------------------------------------------
 * 3. iperf mode — IPERF_TYPE
 * ---------------------------------------------------------------------------
 * If the other halow peer is a linux device, the follow is needed.
 * IPERF_UDP_CLIENT: Run "iperf -s -p {IPERF_SERVER_PORT} -U" on the halow peer before esp32 start, and set IPERF_SERVER_IP to peer IP.
 * IPERF_TCP_CLIENT: Run "iperf -s -p {IPERF_SERVER_PORT}" on the halow peer before esp32 start, and set IPERF_SERVER_IP to peer IP.
 * IPERF_UDP_SERVER: Run "iperf -c {IP} -p {IPERF_SERVER_PORT} -i 1 -u -b 20M" on the halow peer after esp32 started.
 * IPERF_TCP_SERVER: Run "iperf -c {IP} -p {IPERF_SERVER_PORT} -i 1" on the halow peer after esp32 started.
*/
#include <HaLow.h>
#include "mmiperf.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
/* ------------------------ Configuration options ------------------------ */


#define HALOW_ROLE_STA           0
#define HALOW_ROLE_AP            1
#define HALOW_ROLE_MESH_GATEWAY  2
#define HALOW_ROLE_MESH_POINT    3
#define MESH_GATEWAY             HALOW_ROLE_MESH_GATEWAY
#define MESH_POINT               HALOW_ROLE_MESH_POINT
#ifndef HALOW_ROLE
#define HALOW_ROLE      HALOW_ROLE_STA
#endif

/** Iperf configurations. */
enum iperf_type
{
    IPERF_TCP_SERVER,   /**< TCP server (RX) */
    IPERF_UDP_SERVER,   /**< UDP server (RX) */
    IPERF_TCP_CLIENT,   /**< TCP client (TX) */
    IPERF_UDP_CLIENT,   /**< UDP client (TX) */
};
/*
 * Type of iperf instance to start:
 *   IPERF_TCP_SERVER, IPERF_UDP_SERVER, IPERF_TCP_CLIENT, IPERF_UDP_CLIENT
 *
 * Can also be overridden with -DIPERF_TYPE=IPERF_UDP_CLIENT.
 */
#ifndef IPERF_TYPE
#define IPERF_TYPE      IPERF_TCP_SERVER
#endif

#define HALOW_REGION     "US"
#define HALOW_SSID       "HaLow-AP"
#define HALOW_PASSWORD   "heltec.org"

/* AP mode configuration. */
#ifndef HALOW_CHANNEL
#define HALOW_CHANNEL    28
#endif
#ifndef HALOW_BANDWIDTH
#define HALOW_BANDWIDTH  8
#endif
#ifndef HALOW_MAX_STAS
#define HALOW_MAX_STAS   4
#endif

#ifndef HALOW_USE_DHCP
#ifdef HALOW_POINT_USE_DHCP
#define HALOW_USE_DHCP HALOW_POINT_USE_DHCP
#else
#define HALOW_USE_DHCP 1
#endif
#endif
#ifndef HALOW_POINT_USE_DHCP
#define HALOW_POINT_USE_DHCP HALOW_USE_DHCP
#endif

/* AP / mesh gateway network configuration. */
IPAddress gateway_ip(192, 168, 1, 1);
IPAddress netmask(255, 255, 255, 0);
IPAddress dns_primary(8, 8, 8, 8);
IPAddress dns_secondary(8, 8, 4, 4);
IPAddress dhcp_lease_start(192, 168, 1, 100);
IPAddress dhcp_lease_end(192, 168, 1, 150);

/* STA / mesh point static IP. */
IPAddress static_ip(192, 168, 1, 10);

/* Start the embedded DHCP server in AP and mesh gateway modes. */
#ifndef IPERF_ENABLE_DHCP_SERVER
#ifdef IPERF_AP_ENABLE_DHCP_SERVER
#define IPERF_ENABLE_DHCP_SERVER IPERF_AP_ENABLE_DHCP_SERVER
#else
#define IPERF_ENABLE_DHCP_SERVER 1
#endif
#endif

#ifndef IPERF_SERVER_IP
/** IP address of server to connect to when in client mode. */
#if HALOW_ROLE == HALOW_ROLE_AP || HALOW_ROLE == HALOW_ROLE_MESH_GATEWAY
#define IPERF_SERVER_IP                 "192.168.1.100"
#elif HALOW_ROLE == HALOW_ROLE_STA || HALOW_ROLE == HALOW_ROLE_MESH_POINT
#define IPERF_SERVER_IP                 "192.168.1.1"
#endif
#endif
// #define IPERF_TIME_AMOUNT -100
#ifndef IPERF_TIME_AMOUNT
/**
 * Duration for client transfers specified either in seconds or bytes.
 * If this is negative, it specifies a time in seconds; if positive, it
 * specifies the number of bytes to transmit.
 */
#define IPERF_TIME_AMOUNT               -10
#endif
#ifndef IPERF_SERVER_PORT
/** Specifies the port to listen on in server mode. */
#define IPERF_SERVER_PORT               1111
#endif
#ifndef IPERF_UDP_BW_KBPS
/** Target bandwidth for UDP client mode (kbps). */
#define IPERF_UDP_BW_KBPS               10000
#endif

/* ------------------------ End of configuration options ------------------------ */

static const char *halow_role_name(void)
{
#if HALOW_ROLE == HALOW_ROLE_STA
    return "STA";
#elif HALOW_ROLE == HALOW_ROLE_AP
    return "AP";
#elif HALOW_ROLE == HALOW_ROLE_MESH_GATEWAY
    return "mesh_gateway";
#elif HALOW_ROLE == HALOW_ROLE_MESH_POINT
    return "mesh_point";
#else
    return "unknown";
#endif
}

#if (HALOW_ROLE == HALOW_ROLE_AP || HALOW_ROLE == HALOW_ROLE_MESH_GATEWAY) && IPERF_ENABLE_DHCP_SERVER
static bool start_embedded_dhcp_server(void)
{
    if (HaLow.isDhcpServerEnabled())
    {
        return true;
    }
    return HaLow.enableDhcpServer(dhcp_lease_start, dhcp_lease_end, dns_primary);
}
#endif

/** Array of power of 10 unit specifiers. */
static const char units[] = {' ', 'K', 'M', 'G', 'T'};

/**
 * Function to format a given number of bytes into an appropriate SI base. I.e if you give it 1400
 * it will return 1 with unit_index set to 1 for Kilo.
 *
 * @warning This uses power of 10 units (kilo, mega, giga, etc). Not to be confused with power of 2
 *          units (kibi, mebi, gibi, etc).
 *
 * @param[in]   bytes       Original number of bytes
 * @param[out]  unit_index  Index into the @ref units array. Must not be NULL
 *
 * @return Number of bytes formatted to the appropriate unit given by the unit index.
 */
static uint32_t format_bytes(uint64_t bytes, uint8_t *unit_index)
{
    MMOSAL_ASSERT(unit_index != NULL);
    *unit_index = 0;

    while (bytes >= 1000 && *unit_index < 4)
    {
        bytes /= 1000;
        (*unit_index)++;
    }

    return bytes;
}
/**
 * Handle a report at the end of an iperf transfer.
 *
 * @param report    The iperf report.
 * @param arg       Opaque argument specified when iperf was started.
 * @param handle    The iperf instance handle returned when iperf was started.
 */
static void iperf_report_handler(const struct mmiperf_report *report, void *arg,
                                 mmiperf_handle_t handle)
{
    (void)arg;
    (void)handle;

    uint8_t bytes_transferred_unit_index = 0;
    uint32_t bytes_transferred_formatted = format_bytes(report->bytes_transferred,
                                                        &bytes_transferred_unit_index);

    Serial.printf("\r\nIperf Report\r\n");
    Serial.printf("  Remote Address: %s:%d\r\n", report->remote_addr, report->remote_port);
    Serial.printf("  Local Address:  %s:%d\r\n", report->local_addr, report->local_port);
    Serial.printf("  Transferred: %lu %cBytes, duration: %lu ms, bandwidth: %lu kbps\r\n",
           bytes_transferred_formatted, units[bytes_transferred_unit_index],
           report->duration_ms, report->bandwidth_kbitpsec);
    Serial.printf("\r\n");

    if ((report->report_type == MMIPERF_UDP_DONE_SERVER) ||
        (report->report_type == MMIPERF_TCP_DONE_SERVER))
    {
        Serial.printf("Waiting for client to connect...\r\n");
    }
}

/** Start iperf as a TCP client. */
static void start_tcp_client(void)
{
    uint32_t server_port = IPERF_SERVER_PORT;
    struct mmiperf_client_args args = MMIPERF_CLIENT_ARGS_DEFAULT;

    /* Get the Server IP */
    strncpy(args.server_addr, IPERF_SERVER_IP, sizeof(args.server_addr));


    MMOSAL_ASSERT(server_port <= UINT16_MAX);
    args.server_port = server_port;

    int amount = IPERF_TIME_AMOUNT;
    args.amount = amount;
    if (args.amount < 0)
    {
        args.amount *= 100;
    }
    args.report_fn = iperf_report_handler;

    mmiperf_start_tcp_client(&args);
    Serial.printf("\r\nIperf TCP client started, waiting for completion...\r\n");
}

/** Start iperf as a UDP client. */
static void start_udp_client(void)
{
    uint32_t server_port = IPERF_SERVER_PORT;
    struct mmiperf_client_args args = MMIPERF_CLIENT_ARGS_DEFAULT;

    strncpy(args.server_addr, IPERF_SERVER_IP, sizeof(args.server_addr));

    MMOSAL_ASSERT(server_port <= UINT16_MAX);
    args.server_port = server_port;
    args.target_bw = IPERF_UDP_BW_KBPS;
    int amount = IPERF_TIME_AMOUNT;
    args.amount = amount;
    if (args.amount < 0)
    {
        args.amount *= 100;
    }
    args.report_fn = iperf_report_handler;

    mmiperf_start_udp_client(&args);
    Serial.printf("\r\nIperf UDP client started, waiting for completion...\r\n");
}

/** Start iperf as a TCP server. */
static void start_tcp_server(void)
{
    struct mmiperf_server_args args = MMIPERF_SERVER_ARGS_DEFAULT;

    uint32_t local_port = IPERF_SERVER_PORT;
    args.local_port = (uint16_t) local_port;

    args.report_fn = iperf_report_handler;

    mmiperf_handle_t iperf_handle = mmiperf_start_tcp_server(&args);
    if (iperf_handle == NULL)
    {
        Serial.printf("Failed to get local address\r\n");
        return;
    }
    Serial.printf("\r\nIperf TCP server started, waiting for client to connect...\r\n");
    struct mmipal_ip_config ip_config;
    enum mmipal_status status;
    status = mmipal_get_ip_config(&ip_config);
    if (status == MMIPAL_SUCCESS)
    {
        Serial.printf("Execute cmd on AP 'iperf -c %s -p %u -i 1' for IPv4\r\n",
               ip_config.ip_addr, args.local_port);
    }

    struct mmipal_ip6_config ip6_config;
    status = mmipal_get_ip6_config(&ip6_config);
    if (status == MMIPAL_SUCCESS)
    {
        Serial.printf("Execute cmd on AP 'iperf -c %s%%wlan0 -p %u -i 1 -V' for IPv6\r\n",
               ip6_config.ip6_addr[0], args.local_port);
    }
}

/** Start iperf as a UDP server. */
static void start_udp_server(void)
{
    struct mmiperf_server_args args = MMIPERF_SERVER_ARGS_DEFAULT;

    uint32_t local_port = IPERF_SERVER_PORT;
    args.local_port = (uint16_t) local_port;

    args.report_fn = iperf_report_handler;

    mmiperf_handle_t iperf_handle = mmiperf_start_udp_server(&args);
    if (iperf_handle == NULL)
    {
        Serial.printf("Failed to start iperf server\r\n");
        return;
    }

    Serial.printf("\r\nIperf UDP server started, waiting for client to connect...\r\n");
    struct mmipal_ip_config ip_config;
    enum mmipal_status status;
    status = mmipal_get_ip_config(&ip_config);
    if (status == MMIPAL_SUCCESS)
    {
        Serial.printf("Execute cmd on AP 'iperf -c %s -p %u -i 1 -u -b 20M' for IPv4\r\n",
               ip_config.ip_addr, args.local_port);
    }

    struct mmipal_ip6_config ip6_config;
    status = mmipal_get_ip6_config(&ip6_config);
    if (status == MMIPAL_SUCCESS)
    {
        Serial.printf("Execute cmd on AP 'iperf -c %s%%wlan0 -p %u -i 1 -V -u -b 20M' for IPv6\r\n",
               ip6_config.ip6_addr[0], args.local_port);
    }
}

static const char *iperf_type_name(enum iperf_type type)
{
    switch (type)
    {
    case IPERF_TCP_SERVER:
        return "TCP server";
    case IPERF_UDP_SERVER:
        return "UDP server";
    case IPERF_TCP_CLIENT:
        return "TCP client";
    case IPERF_UDP_CLIENT:
        return "UDP client";
    default:
        return "unknown";
    }
}

void setup()
{
  Serial.begin(115200);

  Serial.printf("HaLow role: %s\r\n", halow_role_name());

#if HALOW_ROLE == HALOW_ROLE_STA
  Serial.printf("STA IP mode: %s\r\n", HALOW_USE_DHCP ? "DHCP" : "static");
#elif HALOW_ROLE == HALOW_ROLE_MESH_GATEWAY || HALOW_ROLE == HALOW_ROLE_MESH_POINT
  Serial.printf("Mesh ID: %s, channel: %u, bandwidth: %u MHz\r\n",
                HALOW_SSID, HALOW_CHANNEL, HALOW_BANDWIDTH);
#if HALOW_ROLE == HALOW_ROLE_MESH_POINT
  Serial.printf("Mesh point IP mode: %s\r\n",
                HALOW_USE_DHCP ? "DHCP" : "static");
#endif
#endif
  Serial.printf("Iperf mode: %s\r\n", iperf_type_name((enum iperf_type)IPERF_TYPE));

#ifdef HT_RC3268
  //enable WiFiHalow LDO
  pinMode(HALOW_LDO_CTRL,OUTPUT);
  digitalWrite(HALOW_LDO_CTRL,HALOW_LDO_ENABLE);
#endif

#if HALOW_ROLE == HALOW_ROLE_AP
  if (!HaLow.config(gateway_ip, gateway_ip, netmask, dns_primary, dns_secondary)) {
    Serial.println("HaLow AP static IP configuration failed");
  }

  HaLow.init(HALOW_REGION);
  if (!HaLow.AP(HALOW_SSID, HALOW_PASSWORD, HALOW_CHANNEL,
                HALOW_BANDWIDTH, HALOW_MAX_STAS)) {
    Serial.println("Failed to start HaLow AP");
    return;
  }
#elif HALOW_ROLE == HALOW_ROLE_MESH_GATEWAY
  if (!HaLow.config(gateway_ip, gateway_ip, netmask, dns_primary, dns_secondary)) {
    Serial.println("HaLow mesh gateway static IP configuration failed");
  }

  if (!HaLow.meshBegin(HALOW_SSID, HALOW_PASSWORD, HALOW_CHANNEL, HALOW_BANDWIDTH,
                         HALOW_REGION)) {
    Serial.println("Failed to start mesh");
    return;
  }

#if IPERF_ENABLE_DHCP_SERVER
  if (!start_embedded_dhcp_server()) {
    Serial.println("DHCP server not ready yet, will retry after link up");
  }
#endif
#elif HALOW_ROLE == HALOW_ROLE_MESH_POINT
#if !HALOW_USE_DHCP
  if (!HaLow.config(static_ip, gateway_ip, netmask, dns_primary, dns_secondary)) {
    Serial.println("HaLow mesh point static IP configuration failed");
  }
#endif

  if (!HaLow.meshBegin(HALOW_SSID, HALOW_PASSWORD, HALOW_CHANNEL, HALOW_BANDWIDTH,
                         HALOW_REGION)) {
    Serial.println("Failed to start mesh");
    return;
  }
#else
  Serial.print("Connecting to ");
  Serial.println(HALOW_SSID);
#if !HALOW_USE_DHCP
  if (!HaLow.config(static_ip, gateway_ip, netmask, dns_primary, dns_secondary)) {
    Serial.println("HaLow STA static IP configuration failed");
  }
#endif
  HaLow.init(HALOW_REGION);
  HaLow.begin(HALOW_SSID, HALOW_PASSWORD);
#endif

  while (HaLow.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("HaLow connected!");
  Serial.print("IP address: ");
  Serial.println(HaLow.localIP());
  Serial.print("Mac Address: ");
  Serial.println(HaLow.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(HaLow.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(HaLow.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(HaLow.dnsIP());

#if (HALOW_ROLE == HALOW_ROLE_AP || HALOW_ROLE == HALOW_ROLE_MESH_GATEWAY) && IPERF_ENABLE_DHCP_SERVER
  if (!start_embedded_dhcp_server()) {
    Serial.println("HaLow DHCP server start failed");
  } else {
    Serial.printf("HaLow DHCP server started, lease %s - %s\r\n",
                  dhcp_lease_start.toString().c_str(),
                  dhcp_lease_end.toString().c_str());
  }
#endif

  Serial.printf("\r\n\r\nMorse Iperf Demo\r\n\r\n");
  delay(1000);
  enum iperf_type iperf_mode = IPERF_TYPE;

  switch (iperf_mode)
  {
  case IPERF_TCP_SERVER:
      start_tcp_server();
      break;

  case IPERF_UDP_SERVER:
      start_udp_server();
      break;

  case IPERF_UDP_CLIENT:
      start_udp_client();
      break;

  case IPERF_TCP_CLIENT:
      start_tcp_client();
      break;
  }
}



void loop()
{
}

