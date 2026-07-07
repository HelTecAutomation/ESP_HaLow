/*
usage:
IPERF_UDP_CLIENT: Run "iperf -s -p {IPERF_SERVER_PORT} -U" on the halow gateway before esp32 start,and set IPERF_SERVER_IP to gateway IP.
IPERF_TCP_CLIENT: Run "iperf -s -p {IPERF_SERVER_PORT}" on the halow gateway before  esp32 start,and set IPERF_SERVER_IP to gateway IP.
IPERF_UDP_SERVER: Run "iperf -c {IP} -p {IPERF_SERVER_PORT} -i 1 -u -b 20M" on the halow gateway after  esp32 started.
IPERF_TCP_SERVER: Run "iperf -c {IP} -p {IPERF_SERVER_PORT} -i 1" on the halow gateway after esp32 started.
*/
#include <HaLow.h>
#include "mmiperf.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#define HALOW_REGION     "US"
#define HALOW_AP_SSID    "HaLow-AP"
#define HALOW_PASSWORD   "heltec.org"

/* ------------------------ Configuration options ------------------------ */

/** Iperf configurations. */
enum iperf_type
{
    IPERF_TCP_SERVER,   /**< TCP server (RX) */
    IPERF_UDP_SERVER,    /**< UDP server (RX) */
    IPERF_TCP_CLIENT,   /**< TCP client (TX) */
    IPERF_UDP_CLIENT,   /**< UDP client (TX) */
};
#define IPERF_TYPE IPERF_TCP_SERVER
#ifndef IPERF_TYPE
/** Type of iperf instance to start. */
#define IPERF_TYPE                      IPERF_UDP_CLIENT
#endif

#ifndef IPERF_SERVER_IP
/** IP address of server to connect to when in client mode. */
#define IPERF_SERVER_IP                 "192.168.0.78"
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

/* ------------------------ End of configuration options ------------------------ */

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

void setup()
{
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(HALOW_AP_SSID);
  
#ifdef HT_RC3268
  //enable WiFiHalow LDO
  pinMode(HALOW_LDO_CTRL,OUTPUT);
  digitalWrite(HALOW_LDO_CTRL,HALOW_LDO_ENABLE);
#endif


  HaLow.init(HALOW_REGION);
  HaLow.begin(HALOW_AP_SSID, HALOW_PASSWORD);

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

  Serial.printf("\r\n\r\nMorse Iperf Demo\r\n\r\n");

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
