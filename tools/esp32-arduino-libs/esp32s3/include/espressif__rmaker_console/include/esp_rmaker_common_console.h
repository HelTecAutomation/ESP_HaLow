/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_err.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initialize console
 *
 * Initializes serial console and adds basic commands.
 *
 * @return ESP_OK on success.
 * @return ESP_ERR_NOT_SUPPORTED for the REPL with CONFIG_ESP_CONSOLE_NONE, or on Linux before ESP-IDF v5.5.
 * @return error in case of failures.
 *
 * @note This function does console initialization and registers the rmaker common commands.
 *  If you wish to initialize the console yourself, you can call esp_rmaker_common_register_commands after console init
 * @note With CONFIG_ESP_RMAKER_CONSOLE_USE_REPL, this function calls esp_console_init(). Do not call it first.
 */
esp_err_t esp_rmaker_common_console_init(void);

/**
 * @brief Register rmaker common console commands
 *
 * @return void
 *
 * @note This function must be called after console initialization
 *  If you wish to register commands yourself, you can call this function after console initialization
 *  Should not be called if esp_rmaker_common_console_init is called which does this implicitly
 */
void esp_rmaker_common_register_commands(void);

/* Reference for adding custom console commands:
#include <esp_console.h>

static int command_console_handler(int argc, char *argv[])
{
    // Command code here
}

static void register_console_command()
{
    const esp_console_cmd_t cmd = {
        .command = "<command_name>",
        .help = "<help_details>",
        .func = &command_console_handler,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
*/

#ifdef __cplusplus
}
#endif
