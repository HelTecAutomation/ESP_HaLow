/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <esp_event.h>
#ifdef __cplusplus
extern "C"
{
#endif

/** ESP RainMaker Common Event Base */
ESP_EVENT_DECLARE_BASE(RMAKER_COMMON_EVENT);

typedef enum {
    /** Node reboot has been triggered. The associated event data is the time in seconds
     * (type: uint8_t) after which the node will reboot. Note that this time may not be
     * accurate as the events are received asynchronously.*/
    RMAKER_EVENT_REBOOT,
    /** Wi-Fi credentials reset. Triggered after calling esp_rmaker_wifi_reset() */
    RMAKER_EVENT_WIFI_RESET,
    /** Node reset to factory defaults. Triggered after calling esp_rmaker_factory_reset() */
    RMAKER_EVENT_FACTORY_RESET,
    /** Connected to MQTT Broker */
    RMAKER_MQTT_EVENT_CONNECTED,
    /** Disconnected from MQTT Broker */
    RMAKER_MQTT_EVENT_DISCONNECTED,
    /** MQTT message published successfully.
     * Event data will contain the message ID (integer) of published message.
     */
    RMAKER_MQTT_EVENT_PUBLISHED,
    /** POSIX Timezone Changed. Associated data would be NULL terminated POSIX Timezone
     * Eg. "PST8PDT,M3.2.0,M11.1.0" */
    RMAKER_EVENT_TZ_POSIX_CHANGED,
    /** Timezone Changed. Associated data would be NULL terminated Timezone.
     * Eg. "America/Los_Angeles"
     * Note that whenever this event is received, the RMAKER_EVENT_TZ_POSIX_CHANGED event
     * will also be received, but not necessarily vice versa.
     */
    RMAKER_EVENT_TZ_CHANGED,
    /**
     * MQTT message deleted from the outbox if the message couldn't have been sent and acknowledged.
     * Event data will contain the message ID (integer) of deleted message.
     * Valid only if CONFIG_MQTT_REPORT_DELETED_MESSAGES is enabled.
     */
    RMAKER_MQTT_EVENT_MSG_DELETED,
    /** MQTT subscription acknowledged by the broker. Event data is the NUL-terminated topic.
     * Also posted for every subscription re-established after a reconnect.
     * Note: this and RMAKER_MQTT_EVENT_SUBSCRIBE_FAILED are usually posted while the MQTT task
     * holds its API lock, so a handler must not publish or subscribe directly; defer that to a
     * timer or the work queue.
     */
    RMAKER_MQTT_EVENT_SUBSCRIBED,
    /** MQTT subscription rejected by the broker (SUBACK failure code, e.g. throttling) or
     * dropped without an acknowledgement. Event data is the NUL-terminated topic. The
     * subscription is retried internally with backoff and RMAKER_MQTT_EVENT_SUBSCRIBED
     * follows once it succeeds.
     */
    RMAKER_MQTT_EVENT_SUBSCRIBE_FAILED,
    /** MQTT client reported an error. Posted only when the esp-mqtt backend is selected
     * (CONFIG_ESP_RMAKER_LIB_ESP_MQTT).
     *
     * To read event data, include "mqtt_client.h" and cast event_data to
     * esp_mqtt_error_codes_t *. The data is a copy and is valid only for the duration
     * of the handler.
     *
     * Meaningful fields depend on error_type:
     * - MQTT_ERROR_TYPE_TCP_TRANSPORT: esp_tls_last_esp_err, esp_tls_stack_err,
     *   esp_tls_cert_verify_flags, esp_transport_sock_errno
     * - MQTT_ERROR_TYPE_CONNECTION_REFUSED: connect_return_code
     *
     * This event can be posted on every failed reconnect attempt. Handlers should be
     * cheap and non-blocking.
     */
    RMAKER_MQTT_EVENT_ERROR,
} esp_rmaker_common_event_t;
#ifdef __cplusplus
}
#endif
