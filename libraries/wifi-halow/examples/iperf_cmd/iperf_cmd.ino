/*
 * Iperf command console example.
 *
 * By default the ESP32 starts both a TCP server and a UDP server.
 * Use the serial console to start TCP/UDP client tests:
 *
 *   help
 *   ip
 *   srv
 *   tcp [server_ip] [port]
 *   udp [server_ip] [kbps] [port]
 *   mem
 *
 * Build options:
 *   HALOW_ROLE=HALOW_ROLE_STA             ESP32 joins an existing HaLow AP
 *   HALOW_ROLE=HALOW_ROLE_AP              ESP32 creates a HaLow AP (default)
 *   HALOW_ROLE=HALOW_ROLE_MESH_GATEWAY    ESP32 creates a mesh gateway + DHCP server
 *   HALOW_ROLE=HALOW_ROLE_MESH_POINT      ESP32 joins as a mesh point
 *
 *   ENABLE_DHCP_SERVER=1                  AP / mesh gatway enable DHCP server(default)
 *   ENABLE_DHCP_SERVER=0                  AP / mesh gatway disable DHCP server
 *
 *   HALOW_USE_DHCP=1                      STA / mesh point uses DHCP (default)
 *   HALOW_USE_DHCP=0                      STA / mesh point uses a static IP
 */
#include <HaLow.h>
#include "mmiperf.h"
#include "mmipal.h"
#include "lwip/err.h"
#include "esp_system.h"
#include "esp_heap_caps.h"

/* ------------------------ Configuration options ------------------------ */

#define HALOW_ROLE_STA           0
#define HALOW_ROLE_AP            1
#define HALOW_ROLE_MESH_GATEWAY  2
#define HALOW_ROLE_MESH_POINT    3
#ifndef HALOW_ROLE
#define HALOW_ROLE      HALOW_ROLE_STA
#endif

#define HALOW_REGION     "US"
#define HALOW_SSID       "HaLow-AP"
#define HALOW_PASSWORD   "heltec.org"

// Please read REGION_SW_CHANNEL_REF.h to obtain the relationship between region, bandwidth,  channel and freq.
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
#define HALOW_USE_DHCP 1
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

#ifndef ENABLE_DHCP_SERVER
#define ENABLE_DHCP_SERVER 1
#endif

#ifndef IPERF_SERVER_IP
#if HALOW_ROLE == HALOW_ROLE_AP
/* First address normally handed out by the default AP DHCP pool. */
#define IPERF_SERVER_IP "192.168.1.100"
#elif HALOW_ROLE == HALOW_ROLE_MESH_GATEWAY
#define IPERF_SERVER_IP "192.168.1.100"
#elif HALOW_ROLE == HALOW_ROLE_MESH_POINT
#define IPERF_SERVER_IP "192.168.1.1"
#else
#define IPERF_SERVER_IP "192.168.1.1"
#endif
#endif

#ifndef IPERF_PORT
#define IPERF_PORT 5001
#endif

#ifndef IPERF_DURATION_SECONDS
#define IPERF_DURATION_SECONDS 10
#endif

#ifndef IPERF_UDP_BW_KBPS
#define IPERF_UDP_BW_KBPS 10000
#endif

#ifndef IPERF_TCP_IDLE_TIMEOUT_S
#define IPERF_TCP_IDLE_TIMEOUT_S 15
#endif

/* Set to 0 to disable periodic memory logs. */
#ifndef IPERF_MEM_LOG_INTERVAL_MS
#define IPERF_MEM_LOG_INTERVAL_MS 30000
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

#if (HALOW_ROLE == HALOW_ROLE_AP || HALOW_ROLE == HALOW_ROLE_MESH_GATEWAY) && ENABLE_DHCP_SERVER
static bool start_embedded_dhcp_server(void)
{
    if (HaLow.isDhcpServerEnabled())
    {
        return true;
    }
    return HaLow.enableDhcpServer(dhcp_lease_start, dhcp_lease_end, dns_primary);
}
#endif

static const char *units[] = {" ", "K", "M", "G", "T"};
static mmiperf_handle_t tcp_server_handle = NULL;
static mmiperf_handle_t udp_server_handle = NULL;
static char cmd_line[128];
static size_t cmd_len = 0;

static uint32_t format_bytes(uint64_t bytes, uint8_t *unit_index)
{
    *unit_index = 0;
    while (bytes >= 1000 && *unit_index < 4)
    {
        bytes /= 1000;
        (*unit_index)++;
    }
    return (uint32_t)bytes;
}

static void print_mem(void)
{
    size_t internal_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    size_t internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t internal_used = internal_total > internal_free ? internal_total - internal_free : 0;
    size_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    size_t psram_used = psram_total > psram_free ? psram_total - psram_free : 0;

    Serial.printf("[MEM] heap free=%lu min=%lu\r\n",
                  (unsigned long)esp_get_free_heap_size(),
                  (unsigned long)esp_get_minimum_free_heap_size());
    Serial.printf("[MEM] internal total=%lu used=%lu free=%lu largest=%lu\r\n",
                  (unsigned long)internal_total,
                  (unsigned long)internal_used,
                  (unsigned long)internal_free,
                  (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
    Serial.printf("[MEM] psram total=%lu used=%lu free=%lu largest=%lu\r\n",
                  (unsigned long)psram_total,
                  (unsigned long)psram_used,
                  (unsigned long)psram_free,
                  (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
    Serial.printf("[MEM] dma free=%lu largest=%lu\r\n",
                  (unsigned long)heap_caps_get_free_size(MALLOC_CAP_DMA),
                  (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_DMA));
}

static void iperf_report_handler(const struct mmiperf_report *report, void *arg,
                                 mmiperf_handle_t handle)
{
    (void)arg;
    (void)handle;

    uint8_t unit_index = 0;
    uint32_t formatted = format_bytes(report->bytes_transferred, &unit_index);

    Serial.printf("\r\n[IPERF] type=%u remote=%s:%u local=%s:%u\r\n",
                  (unsigned)report->report_type,
                  report->remote_addr, (unsigned)report->remote_port,
                  report->local_addr, (unsigned)report->local_port);
    Serial.printf("  transferred=%lu %cBytes duration=%lu ms bandwidth=%lu kbps\r\n",
                  (unsigned long)formatted, units[unit_index][0],
                  (unsigned long)report->duration_ms,
                  (unsigned long)report->bandwidth_kbitpsec);

    if (report->report_type == MMIPERF_TCP_DONE_SERVER ||
        report->report_type == MMIPERF_UDP_DONE_SERVER)
    {
        Serial.printf("  waiting for next client...\r\n");
    }
    Serial.printf("\r\n");
}

static void start_tcp_server(void)
{
    if (tcp_server_handle != NULL)
    {
        Serial.println("TCP server already running");
        return;
    }

    struct mmiperf_server_args args = MMIPERF_SERVER_ARGS_DEFAULT;
    args.local_port = IPERF_PORT;
    args.tcp_idle_timeout_s = IPERF_TCP_IDLE_TIMEOUT_S;
    args.report_fn = iperf_report_handler;

    tcp_server_handle = mmiperf_start_tcp_server(&args);
    if (tcp_server_handle == NULL)
    {
        Serial.println("Failed to start TCP server");
        return;
    }
    Serial.printf("TCP server listening on port %u\r\n", IPERF_PORT);
}

static void start_udp_server(void)
{
    if (udp_server_handle != NULL)
    {
        Serial.println("UDP server already running");
        return;
    }

    struct mmiperf_server_args args = MMIPERF_SERVER_ARGS_DEFAULT;
    args.local_port = IPERF_PORT;
    args.report_fn = iperf_report_handler;

    udp_server_handle = mmiperf_start_udp_server(&args);
    if (udp_server_handle == NULL)
    {
        Serial.println("Failed to start UDP server");
        return;
    }
    Serial.printf("UDP server listening on port %u\r\n", IPERF_PORT);
}

static void start_servers(void)
{
    start_tcp_server();
    start_udp_server();
}

static void start_tcp_client(const char *server_ip, uint16_t server_port)
{
    struct mmiperf_client_args args = MMIPERF_CLIENT_ARGS_DEFAULT;

    (void)mmosal_safer_strcpy(args.server_addr, server_ip, sizeof(args.server_addr));
    args.server_port = server_port;
    args.amount = -IPERF_DURATION_SECONDS * 100;
    args.report_fn = iperf_report_handler;

    if (mmiperf_start_tcp_client(&args) == NULL)
    {
        Serial.printf("Failed to start TCP client to %s:%u\r\n", server_ip, server_port);
        return;
    }
    Serial.printf("TCP client started to %s:%u for %u s\r\n",
                  server_ip, server_port, IPERF_DURATION_SECONDS);
}

static void start_udp_client(const char *server_ip, uint32_t bandwidth_kbps,
                             uint16_t server_port)
{
    struct mmiperf_client_args args = MMIPERF_CLIENT_ARGS_DEFAULT;

    (void)mmosal_safer_strcpy(args.server_addr, server_ip, sizeof(args.server_addr));
    args.server_port = server_port;
    args.target_bw = bandwidth_kbps;
    args.amount = -IPERF_DURATION_SECONDS * 100;
    args.report_fn = iperf_report_handler;

    if (mmiperf_start_udp_client(&args) == NULL)
    {
        Serial.printf("Failed to start UDP client to %s:%u\r\n", server_ip, server_port);
        return;
    }
    Serial.printf("UDP client started to %s:%u for %u s, %lu kbps\r\n",
                  server_ip, server_port, IPERF_DURATION_SECONDS,
                  (unsigned long)bandwidth_kbps);
}

static void print_ip(void)
{
    struct mmipal_ip_config config;

    if (mmipal_get_ip_config(&config) == MMIPAL_SUCCESS)
    {
        Serial.printf("IP: %s netmask: %s gateway: %s\r\n",
                      config.ip_addr, config.netmask, config.gateway_addr);
    }
    else
    {
        Serial.println("IP address not available yet");
    }

    Serial.printf("MAC: %s\r\n", HaLow.macAddress().c_str());
}

static void print_help(void)
{
    Serial.println();
    Serial.printf("HaLow role: %s, local IP: %s, port: %u\r\n",
                  halow_role_name(),
                  HaLow.localIP().toString().c_str(), IPERF_PORT);
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
    Serial.println("Commands:");
    Serial.println("  help                     show this help");
    Serial.println("  ip                       show IP configuration");
    Serial.println("  srv                      start TCP and UDP servers");
    Serial.printf("  tcp [server_ip] [port]        start TCP client (%u s), default %s:%u\r\n",
                  IPERF_DURATION_SECONDS, IPERF_SERVER_IP, IPERF_PORT);
    Serial.printf("  udp [server_ip] [kbps] [port] start UDP client (%u s), default %s:%u %u kbps\r\n",
                  IPERF_DURATION_SECONDS, IPERF_SERVER_IP, IPERF_PORT, IPERF_UDP_BW_KBPS);
    Serial.println("  mem                      show heap/PSRAM usage");
    Serial.println();
}

static void handle_command(char *line)
{
    char cmd[16] = {0};
    char ip[48] = {0};
    int arg2 = 0;
    int arg3 = 0;
    int tokens = sscanf(line, "%15s %47s %d %d", cmd, ip, &arg2, &arg3);

    if (tokens < 1)
    {
        return;
    }

    if (strcmp(cmd, "help") == 0)
    {
        print_help();
    }
    else if (strcmp(cmd, "ip") == 0)
    {
        print_ip();
    }
    else if (strcmp(cmd, "srv") == 0)
    {
        start_servers();
    }
    else if (strcmp(cmd, "tcp") == 0)
    {
        int port = tokens >= 3 ? arg2 : IPERF_PORT;
        if (port < 1 || port > 65535)
        {
            Serial.println("Invalid TCP port");
            return;
        }
        start_tcp_client(tokens >= 2 ? ip : IPERF_SERVER_IP, (uint16_t)port);
    }
    else if (strcmp(cmd, "udp") == 0)
    {
        int bandwidth = tokens >= 3 ? arg2 : IPERF_UDP_BW_KBPS;
        int port = tokens >= 4 ? arg3 : IPERF_PORT;
        if (bandwidth <= 0 || port < 1 || port > 65535)
        {
            Serial.println("Invalid UDP port or bandwidth");
            return;
        }
        start_udp_client(tokens >= 2 ? ip : IPERF_SERVER_IP,
                         (uint32_t)bandwidth, (uint16_t)port);
    }
    else if (strcmp(cmd, "mem") == 0)
    {
        print_mem();
    }
    else
    {
        Serial.printf("Unknown command: %s (type 'help')\r\n", cmd);
    }
}

void setup()
{
    Serial.begin(115200);

    Serial.printf("\r\nIperf command example, role=%s\r\n",
                  halow_role_name());

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

#ifdef HT_RC3268
    pinMode(HALOW_LDO_CTRL, OUTPUT);
    digitalWrite(HALOW_LDO_CTRL, HALOW_LDO_ENABLE);
#endif

#if HALOW_ROLE == HALOW_ROLE_AP
    if (!HaLow.config(gateway_ip, gateway_ip, netmask,
                      dns_primary, dns_secondary))
    {
        Serial.println("HaLow AP static IP configuration failed");
    }

    HaLow.init(HALOW_REGION);
    if (!HaLow.AP(HALOW_SSID, HALOW_PASSWORD, HALOW_CHANNEL,
                  HALOW_BANDWIDTH, HALOW_MAX_STAS))
    {
        Serial.println("Failed to start HaLow AP");
        return;
    }
#elif HALOW_ROLE == HALOW_ROLE_MESH_GATEWAY
    if (!HaLow.config(gateway_ip, gateway_ip, netmask,
                      dns_primary, dns_secondary))
    {
        Serial.println("HaLow mesh gateway static IP configuration failed");
    }

    if (!HaLow.meshBegin(HALOW_SSID, HALOW_PASSWORD, HALOW_CHANNEL, HALOW_BANDWIDTH,
                         HALOW_REGION))
    {
        Serial.println("Failed to start mesh");
        return;
    }

#if ENABLE_DHCP_SERVER
    if (!start_embedded_dhcp_server())
    {
        Serial.println("DHCP server not ready yet, will retry after link up");
    }
#endif
#elif HALOW_ROLE == HALOW_ROLE_MESH_POINT
#if !HALOW_USE_DHCP
    if (!HaLow.config(static_ip, gateway_ip, netmask,
                      dns_primary, dns_secondary))
    {
        Serial.println("HaLow mesh point static IP configuration failed");
    }
#endif

    if (!HaLow.meshBegin(HALOW_SSID, HALOW_PASSWORD, HALOW_CHANNEL, HALOW_BANDWIDTH,
                         HALOW_REGION))
    {
        Serial.println("Failed to start mesh");
        return;
    }
#else
    Serial.print("Connecting to ");
    Serial.println(HALOW_SSID);
#if !HALOW_USE_DHCP
    if (!HaLow.config(static_ip, gateway_ip, netmask,
                      dns_primary, dns_secondary))
    {
        Serial.println("HaLow STA static IP configuration failed");
    }
#endif
    HaLow.init(HALOW_REGION);
    HaLow.begin(HALOW_SSID, HALOW_PASSWORD);
#endif

    while (HaLow.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("HaLow connected!");
    print_ip();

#if (HALOW_ROLE == HALOW_ROLE_AP || HALOW_ROLE == HALOW_ROLE_MESH_GATEWAY) && ENABLE_DHCP_SERVER
    if (!start_embedded_dhcp_server())
    {
        Serial.println("HaLow DHCP server start failed");
    }
    else
    {
        Serial.printf("HaLow DHCP server started, lease %s - %s\r\n",
                      dhcp_lease_start.toString().c_str(),
                      dhcp_lease_end.toString().c_str());
    }
#endif

    start_servers();
    Serial.println("Initial memory usage:");
    print_help();
}

void loop()
{
#if IPERF_MEM_LOG_INTERVAL_MS > 0
    static uint32_t last_mem_log = 0;
#endif

    while (Serial.available())
    {
        int ch = Serial.read();
        if (ch == '\r')
        {
            continue;
        }
        if (ch == '\n')
        {
            cmd_line[cmd_len] = '\0';
            if (cmd_len > 0)
            {
                Serial.printf("iperf> %s\r\n", cmd_line);
                handle_command(cmd_line);
            }
            cmd_len = 0;
        }
        else if ((ch == 0x7f || ch == '\b') && cmd_len > 0)
        {
            cmd_len--;
        }
        else if (cmd_len < sizeof(cmd_line) - 1)
        {
            cmd_line[cmd_len++] = (char)ch;
        }
    }

    delay(10);
}
