/*
 * Copyright 2023 Morse Micro
 *
 * This file is licensed under terms that can be found in the LICENSE.md file in the root
 * directory of the Morse Micro IoT SDK software package.
 */

/**
 * @ingroup MMCONFIG
 * @defgroup LOADCONFIG Helper functions for loading configuration
 *
 * This API provides a set of helper functions for loading configuration from @ref MMCONFIG.
 *
 * @{
 */

#include "mmipal.h"
#include "mmwlan.h"
#include "halow_config.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Looks up country code and returns appropriate channel list.
 *
 * @returns A pointer to the channel list to load.
 */
const struct mmwlan_s1g_channel_list* load_channel_list(void);

/**
 * Loads the provided structure with initialization parameters
 * read from config store.  If a specific parameter is not found then
 * default values are used.  Use this function to load defaults before
 * calling @c mmipal_init().
 *
 * @param args  A pointer to the @c mmipal_init_args to return
 *              the settings in.
 */
void load_mmipal_init_args(struct mmipal_init_args *args);

/**
 * Loads the provided structure with initialization parameters
 * read from config store.  If a specific parameter is not found then
 * default values are used.  Use this function to load defaults before
 * calling @c mmwlan_sta_enable().
 *
 * @param sta_config A pointer to the @c mmwlan_sta_args to return
 *                   the settings in.
 */
void load_mmwlan_sta_args(struct mmwlan_sta_args *sta_config);

/**
 * Loads the following settings from config store and applies them.
 * @c wlan.subbands_enabled, @c wlan.sgi_enabled, @c wlan.ampdu_enabled,
 * @c wlan.fragment_threshold and @c wlan.rts_threshold.
 */
void load_mmwlan_settings(void);

/** @} */

/**
 * @ingroup LOADCONFIG
 * @defgroup LOADCONFIG_DEPRECATED Deprecated API for loading configuration
 *
 * This API provides a set of helper functions for loading configuration from @ref MMCONFIG.
 *
 * @deprecated This API is deprecated and should not be used in new developments.
 *
 * @{
 */

/* Include lwip_mmwlan.h for implementation. */
struct lwip_mmwlan_init_args;

/**
 * Loads the provided structure with initialization parameters
 * read from config store.  If a specific parameter is not found then
 * default values are used.  Use this function to load defaults before
 * calling @c lwip_mmwlan_init().
 *
 * @param mmwlan_config A pointer to the @c lwip_mmwlan_init_args to return
 *                      the settings in.
 *
 * @deprecated This function is deprecated and may be removed in future.
 *             @ref load_mmipal_init_args() should be used instead.
 */
void load_lwip_mmwlan_init_args(struct lwip_mmwlan_init_args *mmwlan_config);

#ifdef __cplusplus
}
#endif

/** @} */
