/*
 *
 * Copyright 2022-2023 Morse Micro
 */
/**
 * @ingroup MMWLAN_REGDB
 * @defgroup MMWLAN_REGDB_TEMPLATE Template S1G regulatory database
 *
 * \{
 *
 * @section MMWLAN_REGDB_TEMPLATE_DISCLAIMER Disclaimer
 *
 * While every effort has been made to maintain accuracy of this database, no guarantee is
 * given as to the accuracy of the information contained herein.
 *
 * @section MMWLAN_REGDB_TEMPLATE_COUNTRIES Country code list
 *
 * | Country Code | Country |
 * | ------------ | ------- |
 * | AL | Albania |
 * | AR | Argentina |
 * | AT | Austria |
 * | AU | Australia |
 * | BA | Bosnia and Herzegovina |
 * | BE | Belgium |
 * | BG | Bulgaria |
 * | BR | Brazil |
 * | CA | Canada |
 * | CH | Switzerland |
 * | CY | Cypress |
 * | CZ | Czech Republic |
 * | DD | Germany |
 * | DK | Denmark |
 * | EE | Estonia |
 * | ES | Spain |
 * | EU | EU |
 * | FI | Finland |
 * | FL | Liechtenstein |
 * | FR | France |
 * | GR | Greece |
 * | HR | Croatia |
 * | HU | Hungary |
 * | IE | Ireland |
 * | IN | India |
 * | IS | Iceland |
 * | IT | Italy |
 * | JP | Japan |
 * | KE | Kenya |
 * | KR | South Korea |
 * | LT | Lithuania |
 * | LU | Luxembourg |
 * | LV | Latvia |
 * | MD | Moldova |
 * | MK | North Macedonia |
 * | MT | Malta |
 * | NL | Netherlands |
 * | NO | Norway |
 * | NZ | New Zealand |
 * | PL | Poland |
 * | PT | Portugal |
 * | RO | Romania |
 * | SE | Sweden |
 * | SG | Singapore |
 * | SI | Slovenia |
 * | SK | Slovakia |
 * | TR | Turkey |
 * | UK | United Kingdom |
 * | US | USA |
 */
#ifndef __MMWLAN_REGDB_H__
#define __MMWLAN_REGDB_H__
#include "mmwlan.h"
#include "halow_config.h"
/** List of valid S1G channels for Albania. */
static const struct mmwlan_s1g_channel s1g_channels_AL[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Albania. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_AL = {
    .country_code = "AL",
    .num_channels = (sizeof(s1g_channels_AL)/sizeof(s1g_channels_AL[0])),
    .channels = s1g_channels_AL,
};

/** List of valid S1G channels for Argentina. */
static const struct mmwlan_s1g_channel s1g_channels_AR[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  915500000, 10000, false, 68,  1, 27,  1,  36, 0, 0, 0 },
    {  916500000, 10000, false, 68,  1, 29,  1,  36, 0, 0, 0 },
    {  917500000, 10000, false, 68,  1, 31,  1,  36, 0, 0, 0 },
    {  918500000, 10000, false, 68,  1, 33,  1,  36, 0, 0, 0 },
    {  919500000, 10000, false, 68,  1, 35,  1,  36, 0, 0, 0 },
    {  920500000, 10000, false, 68,  1, 37,  1,  36, 0, 0, 0 },
    {  921500000, 10000, false, 68,  1, 39,  1,  36, 0, 0, 0 },
    {  922500000, 10000, false, 68,  1, 41,  1,  36, 0, 0, 0 },
    {  923500000, 10000, false, 68,  1, 43,  1,  36, 0, 0, 0 },
    {  924500000, 10000, false, 68,  1, 45,  1,  36, 0, 0, 0 },
    {  925500000, 10000, false, 68,  1, 47,  1,  36, 0, 0, 0 },
    {  926500000, 10000, false, 68,  1, 49,  1,  36, 0, 0, 0 },
    {  927500000, 10000, false, 68,  1, 51,  1,  36, 0, 0, 0 },
    {  917000000, 10000, false, 69,  2, 30,  2,  36, 0, 0, 0 },
    {  919000000, 10000, false, 69,  2, 34,  2,  36, 0, 0, 0 },
    {  921000000, 10000, false, 69,  2, 38,  2,  36, 0, 0, 0 },
    {  923000000, 10000, false, 69,  2, 42,  2,  36, 0, 0, 0 },
    {  925000000, 10000, false, 69,  2, 46,  2,  36, 0, 0, 0 },
    {  927000000, 10000, false, 69,  2, 50,  2,  36, 0, 0, 0 },
    {  918000000, 10000, false, 70,  3, 32,  4,  36, 0, 0, 0 },
    {  922000000, 10000, false, 70,  3, 40,  4,  36, 0, 0, 0 },
    {  926000000, 10000, false, 70,  3, 48,  4,  36, 0, 0, 0 },
    {  924000000, 10000, false, 71,  4, 44,  8,  36, 0, 0, 0 },
};

/** Channel list structure for Argentina. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_AR = {
    .country_code = "AR",
    .num_channels = (sizeof(s1g_channels_AR)/sizeof(s1g_channels_AR[0])),
    .channels = s1g_channels_AR,
};

/** List of valid S1G channels for Austria. */
static const struct mmwlan_s1g_channel s1g_channels_AT[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Austria. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_AT = {
    .country_code = "AT",
    .num_channels = (sizeof(s1g_channels_AT)/sizeof(s1g_channels_AT[0])),
    .channels = s1g_channels_AT,
};

/** List of valid S1G channels for Australia. */
static const struct mmwlan_s1g_channel s1g_channels_AU[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  915500000, 10000, false, 68, 22, 27,  1,  30, 0, 0, 0 },
    {  916500000, 10000, false, 68, 22, 29,  1,  30, 0, 0, 0 },
    {  917500000, 10000, false, 68, 22, 31,  1,  30, 0, 0, 0 },
    {  918500000, 10000, false, 68, 22, 33,  1,  30, 0, 0, 0 },
    {  919500000, 10000, false, 68, 22, 35,  1,  30, 0, 0, 0 },
    {  920500000, 10000, false, 68, 22, 37,  1,  30, 0, 0, 0 },
    {  921500000, 10000, false, 68, 22, 39,  1,  30, 0, 0, 0 },
    {  922500000, 10000, false, 68, 22, 41,  1,  30, 0, 0, 0 },
    {  923500000, 10000, false, 68, 22, 43,  1,  30, 0, 0, 0 },
    {  924500000, 10000, false, 68, 22, 45,  1,  30, 0, 0, 0 },
    {  925500000, 10000, false, 68, 22, 47,  1,  30, 0, 0, 0 },
    {  926500000, 10000, false, 68, 22, 49,  1,  30, 0, 0, 0 },
    {  927500000, 10000, false, 68, 22, 51,  1,  30, 0, 0, 0 },
    {  917000000, 10000, false, 69, 23, 30,  2,  30, 0, 0, 0 },
    {  919000000, 10000, false, 69, 23, 34,  2,  30, 0, 0, 0 },
    {  921000000, 10000, false, 69, 23, 38,  2,  30, 0, 0, 0 },
    {  923000000, 10000, false, 69, 23, 42,  2,  30, 0, 0, 0 },
    {  925000000, 10000, false, 69, 23, 46,  2,  30, 0, 0, 0 },
    {  927000000, 10000, false, 69, 23, 50,  2,  30, 0, 0, 0 },
    {  918000000, 10000, false, 70, 24, 32,  4,  30, 0, 0, 0 },
    {  922000000, 10000, false, 70, 24, 40,  4,  30, 0, 0, 0 },
    {  926000000, 10000, false, 70, 24, 48,  4,  30, 0, 0, 0 },
    {  924000000, 10000, false, 71, 25, 44,  8,  30, 0, 0, 0 },
};

/** Channel list structure for Australia. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_AU = {
    .country_code = "AU",
    .num_channels = (sizeof(s1g_channels_AU)/sizeof(s1g_channels_AU[0])),
    .channels = s1g_channels_AU,
};

/** List of valid S1G channels for Bosnia and Herzegovina. */
static const struct mmwlan_s1g_channel s1g_channels_BA[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Bosnia and Herzegovina. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_BA = {
    .country_code = "BA",
    .num_channels = (sizeof(s1g_channels_BA)/sizeof(s1g_channels_BA[0])),
    .channels = s1g_channels_BA,
};

/** List of valid S1G channels for Belgium. */
static const struct mmwlan_s1g_channel s1g_channels_BE[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  916900000,   280, false, 77, 30, 31,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Belgium. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_BE = {
    .country_code = "BE",
    .num_channels = (sizeof(s1g_channels_BE)/sizeof(s1g_channels_BE[0])),
    .channels = s1g_channels_BE,
};

/** List of valid S1G channels for Bulgaria. */
static const struct mmwlan_s1g_channel s1g_channels_BG[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Bulgaria. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_BG = {
    .country_code = "BG",
    .num_channels = (sizeof(s1g_channels_BG)/sizeof(s1g_channels_BG[0])),
    .channels = s1g_channels_BG,
};

/** List of valid S1G channels for Brazil. */
static const struct mmwlan_s1g_channel s1g_channels_BR[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  902500000, 10000, false, 68,  1,  1,  1,  36, 0, 0, 0 }, /* Warning: regulatory requirements may not be met */
    {  903500000, 10000, false, 68,  1,  3,  1,  36, 0, 0, 0 },
    {  904500000, 10000, false, 68,  1,  5,  1,  36, 0, 0, 0 },
    {  905500000, 10000, false, 68,  1,  7,  1,  36, 0, 0, 0 },
    {  906500000, 10000, false, 68,  1,  9,  1,  36, 0, 0, 0 },
    {  903000000, 10000, false, 69,  2,  2,  2,  36, 0, 0, 0 }, /* Warning: regulatory requirements may not be met */
    {  905000000, 10000, false, 69,  2,  6,  2,  36, 0, 0, 0 },
    {  915500000, 10000, false, 68,  1, 27,  1,  36, 0, 0, 0 },
    {  916500000, 10000, false, 68,  1, 29,  1,  36, 0, 0, 0 },
    {  917500000, 10000, false, 68,  1, 31,  1,  36, 0, 0, 0 },
    {  918500000, 10000, false, 68,  1, 33,  1,  36, 0, 0, 0 },
    {  919500000, 10000, false, 68,  1, 35,  1,  36, 0, 0, 0 },
    {  920500000, 10000, false, 68,  1, 37,  1,  36, 0, 0, 0 },
    {  921500000, 10000, false, 68,  1, 39,  1,  36, 0, 0, 0 },
    {  922500000, 10000, false, 68,  1, 41,  1,  36, 0, 0, 0 },
    {  923500000, 10000, false, 68,  1, 43,  1,  36, 0, 0, 0 },
    {  924500000, 10000, false, 68,  1, 45,  1,  36, 0, 0, 0 },
    {  925500000, 10000, false, 68,  1, 47,  1,  36, 0, 0, 0 },
    {  926500000, 10000, false, 68,  1, 49,  1,  36, 0, 0, 0 },
    {  927500000, 10000, false, 68,  1, 51,  1,  36, 0, 0, 0 },
    {  917000000, 10000, false, 69,  2, 30,  2,  36, 0, 0, 0 },
    {  919000000, 10000, false, 69,  2, 34,  2,  36, 0, 0, 0 },
    {  921000000, 10000, false, 69,  2, 38,  2,  36, 0, 0, 0 },
    {  923000000, 10000, false, 69,  2, 42,  2,  36, 0, 0, 0 },
    {  925000000, 10000, false, 69,  2, 46,  2,  36, 0, 0, 0 },
    {  927000000, 10000, false, 69,  2, 50,  2,  36, 0, 0, 0 },
    {  918000000, 10000, false, 70,  3, 32,  4,  36, 0, 0, 0 },
    {  922000000, 10000, false, 70,  3, 40,  4,  36, 0, 0, 0 },
    {  926000000, 10000, false, 70,  3, 48,  4,  36, 0, 0, 0 },
    {  924000000, 10000, false, 71,  4, 44,  8,  36, 0, 0, 0 },
};

/** Channel list structure for Brazil. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_BR = {
    .country_code = "BR",
    .num_channels = (sizeof(s1g_channels_BR)/sizeof(s1g_channels_BR[0])),
    .channels = s1g_channels_BR,
};

/** List of valid S1G channels for Canada. */
static const struct mmwlan_s1g_channel s1g_channels_CA[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  902500000, 10000, false, 68,  1,  1,  1,  36, 0, 0, 0 }, /* Warning: regulatory requirements may not be met */
    {  903500000, 10000, false, 68,  1,  3,  1,  36, 0, 0, 0 },
    {  904500000, 10000, false, 68,  1,  5,  1,  36, 0, 0, 0 },
    {  905500000, 10000, false, 68,  1,  7,  1,  36, 0, 0, 0 },
    {  906500000, 10000, false, 68,  1,  9,  1,  36, 0, 0, 0 },
    {  907500000, 10000, false, 68,  1, 11,  1,  36, 0, 0, 0 },
    {  908500000, 10000, false, 68,  1, 13,  1,  36, 0, 0, 0 },
    {  909500000, 10000, false, 68,  1, 15,  1,  36, 0, 0, 0 },
    {  910500000, 10000, false, 68,  1, 17,  1,  36, 0, 0, 0 },
    {  911500000, 10000, false, 68,  1, 19,  1,  36, 0, 0, 0 },
    {  912500000, 10000, false, 68,  1, 21,  1,  36, 0, 0, 0 },
    {  913500000, 10000, false, 68,  1, 23,  1,  36, 0, 0, 0 },
    {  914500000, 10000, false, 68,  1, 25,  1,  36, 0, 0, 0 },
    {  915500000, 10000, false, 68,  1, 27,  1,  36, 0, 0, 0 },
    {  916500000, 10000, false, 68,  1, 29,  1,  36, 0, 0, 0 },
    {  917500000, 10000, false, 68,  1, 31,  1,  36, 0, 0, 0 },
    {  918500000, 10000, false, 68,  1, 33,  1,  36, 0, 0, 0 },
    {  919500000, 10000, false, 68,  1, 35,  1,  36, 0, 0, 0 },
    {  920500000, 10000, false, 68,  1, 37,  1,  36, 0, 0, 0 },
    {  921500000, 10000, false, 68,  1, 39,  1,  36, 0, 0, 0 },
    {  922500000, 10000, false, 68,  1, 41,  1,  36, 0, 0, 0 },
    {  923500000, 10000, false, 68,  1, 43,  1,  36, 0, 0, 0 },
    {  924500000, 10000, false, 68,  1, 45,  1,  36, 0, 0, 0 },
    {  925500000, 10000, false, 68,  1, 47,  1,  36, 0, 0, 0 },
    {  926500000, 10000, false, 68,  1, 49,  1,  36, 0, 0, 0 },
    {  927500000, 10000, false, 68,  1, 51,  1,  36, 0, 0, 0 },
    {  903000000, 10000, false, 69,  2,  2,  2,  36, 0, 0, 0 }, /* Warning: regulatory requirements may not be met */
    {  905000000, 10000, false, 69,  2,  6,  2,  36, 0, 0, 0 },
    {  907000000, 10000, false, 69,  2, 10,  2,  36, 0, 0, 0 },
    {  909000000, 10000, false, 69,  2, 14,  2,  36, 0, 0, 0 },
    {  911000000, 10000, false, 69,  2, 18,  2,  36, 0, 0, 0 },
    {  913000000, 10000, false, 69,  2, 22,  2,  36, 0, 0, 0 },
    {  915000000, 10000, false, 69,  2, 26,  2,  36, 0, 0, 0 },
    {  917000000, 10000, false, 69,  2, 30,  2,  36, 0, 0, 0 },
    {  919000000, 10000, false, 69,  2, 34,  2,  36, 0, 0, 0 },
    {  921000000, 10000, false, 69,  2, 38,  2,  36, 0, 0, 0 },
    {  923000000, 10000, false, 69,  2, 42,  2,  36, 0, 0, 0 },
    {  925000000, 10000, false, 69,  2, 46,  2,  36, 0, 0, 0 },
    {  927000000, 10000, false, 69,  2, 50,  2,  36, 0, 0, 0 },
    {  906000000, 10000, false, 70,  3,  8,  4,  36, 0, 0, 0 },
    {  910000000, 10000, false, 70,  3, 16,  4,  36, 0, 0, 0 },
    {  914000000, 10000, false, 70,  3, 24,  4,  36, 0, 0, 0 },
    {  918000000, 10000, false, 70,  3, 32,  4,  36, 0, 0, 0 },
    {  922000000, 10000, false, 70,  3, 40,  4,  36, 0, 0, 0 },
    {  926000000, 10000, false, 70,  3, 48,  4,  36, 0, 0, 0 },
    {  908000000, 10000, false, 71,  4, 12,  8,  36, 0, 0, 0 },
    {  916000000, 10000, false, 71,  4, 28,  8,  36, 0, 0, 0 },
    {  924000000, 10000, false, 71,  4, 44,  8,  36, 0, 0, 0 },
};

/** Channel list structure for Canada. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_CA = {
    .country_code = "CA",
    .num_channels = (sizeof(s1g_channels_CA)/sizeof(s1g_channels_CA[0])),
    .channels = s1g_channels_CA,
};

/** List of valid S1G channels for Switzerland. */
static const struct mmwlan_s1g_channel s1g_channels_CH[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Switzerland. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_CH = {
    .country_code = "CH",
    .num_channels = (sizeof(s1g_channels_CH)/sizeof(s1g_channels_CH[0])),
    .channels = s1g_channels_CH,
};

/** List of valid S1G channels for Cypress. */
static const struct mmwlan_s1g_channel s1g_channels_CY[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Cypress. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_CY = {
    .country_code = "CY",
    .num_channels = (sizeof(s1g_channels_CY)/sizeof(s1g_channels_CY[0])),
    .channels = s1g_channels_CY,
};

/** List of valid S1G channels for Czech Republic. */
static const struct mmwlan_s1g_channel s1g_channels_CZ[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Czech Republic. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_CZ = {
    .country_code = "CZ",
    .num_channels = (sizeof(s1g_channels_CZ)/sizeof(s1g_channels_CZ[0])),
    .channels = s1g_channels_CZ,
};

/** List of valid S1G channels for Germany. */
static const struct mmwlan_s1g_channel s1g_channels_DD[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Germany. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_DD = {
    .country_code = "DD",
    .num_channels = (sizeof(s1g_channels_DD)/sizeof(s1g_channels_DD[0])),
    .channels = s1g_channels_DD,
};

/** List of valid S1G channels for Denmark. */
static const struct mmwlan_s1g_channel s1g_channels_DK[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Denmark. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_DK = {
    .country_code = "DK",
    .num_channels = (sizeof(s1g_channels_DK)/sizeof(s1g_channels_DK[0])),
    .channels = s1g_channels_DK,
};

/** List of valid S1G channels for Estonia. */
static const struct mmwlan_s1g_channel s1g_channels_EE[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Estonia. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_EE = {
    .country_code = "EE",
    .num_channels = (sizeof(s1g_channels_EE)/sizeof(s1g_channels_EE[0])),
    .channels = s1g_channels_EE,
};

/** List of valid S1G channels for Spain. */
static const struct mmwlan_s1g_channel s1g_channels_ES[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Spain. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_ES = {
    .country_code = "ES",
    .num_channels = (sizeof(s1g_channels_ES)/sizeof(s1g_channels_ES[0])),
    .channels = s1g_channels_ES,
};

/** List of valid S1G channels for EU. */
static const struct mmwlan_s1g_channel s1g_channels_EU[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for EU. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_EU = {
    .country_code = "EU",
    .num_channels = (sizeof(s1g_channels_EU)/sizeof(s1g_channels_EU[0])),
    .channels = s1g_channels_EU,
};

/** List of valid S1G channels for Finland. */
static const struct mmwlan_s1g_channel s1g_channels_FI[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Finland. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_FI = {
    .country_code = "FI",
    .num_channels = (sizeof(s1g_channels_FI)/sizeof(s1g_channels_FI[0])),
    .channels = s1g_channels_FI,
};

/** List of valid S1G channels for Liechtenstein. */
static const struct mmwlan_s1g_channel s1g_channels_FL[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Liechtenstein. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_FL = {
    .country_code = "FL",
    .num_channels = (sizeof(s1g_channels_FL)/sizeof(s1g_channels_FL[0])),
    .channels = s1g_channels_FL,
};

/** List of valid S1G channels for France. */
static const struct mmwlan_s1g_channel s1g_channels_FR[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for France. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_FR = {
    .country_code = "FR",
    .num_channels = (sizeof(s1g_channels_FR)/sizeof(s1g_channels_FR[0])),
    .channels = s1g_channels_FR,
};

/** List of valid S1G channels for Greece. */
static const struct mmwlan_s1g_channel s1g_channels_GR[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Greece. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_GR = {
    .country_code = "GR",
    .num_channels = (sizeof(s1g_channels_GR)/sizeof(s1g_channels_GR[0])),
    .channels = s1g_channels_GR,
};

/** List of valid S1G channels for Croatia. */
static const struct mmwlan_s1g_channel s1g_channels_HR[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Croatia. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_HR = {
    .country_code = "HR",
    .num_channels = (sizeof(s1g_channels_HR)/sizeof(s1g_channels_HR[0])),
    .channels = s1g_channels_HR,
};

/** List of valid S1G channels for Hungary. */
static const struct mmwlan_s1g_channel s1g_channels_HU[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Hungary. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_HU = {
    .country_code = "HU",
    .num_channels = (sizeof(s1g_channels_HU)/sizeof(s1g_channels_HU[0])),
    .channels = s1g_channels_HU,
};

/** List of valid S1G channels for Ireland. */
static const struct mmwlan_s1g_channel s1g_channels_IE[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  916900000,   280, false, 77, 30, 31,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Ireland. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_IE = {
    .country_code = "IE",
    .num_channels = (sizeof(s1g_channels_IE)/sizeof(s1g_channels_IE[0])),
    .channels = s1g_channels_IE,
};

/** List of valid S1G channels for India. */
static const struct mmwlan_s1g_channel s1g_channels_IN[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for India. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_IN = {
    .country_code = "IN",
    .num_channels = (sizeof(s1g_channels_IN)/sizeof(s1g_channels_IN[0])),
    .channels = s1g_channels_IN,
};

/** List of valid S1G channels for Iceland. */
static const struct mmwlan_s1g_channel s1g_channels_IS[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  916900000,   280, false, 77, 30, 31,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Iceland. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_IS = {
    .country_code = "IS",
    .num_channels = (sizeof(s1g_channels_IS)/sizeof(s1g_channels_IS[0])),
    .channels = s1g_channels_IS,
};

/** List of valid S1G channels for Italy. */
static const struct mmwlan_s1g_channel s1g_channels_IT[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Italy. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_IT = {
    .country_code = "IT",
    .num_channels = (sizeof(s1g_channels_IT)/sizeof(s1g_channels_IT[0])),
    .channels = s1g_channels_IT,
};

/** List of valid S1G channels for Japan. */
static const struct mmwlan_s1g_channel s1g_channels_JP[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  923000000,  1000, true, 73,  8, 13,  1,  16, 2000, 2000, 100000 },
    {  924000000,  1000, true, 73,  8, 15,  1,  16, 2000, 2000, 100000 },
    {  925000000,  1000, true, 73,  8, 17,  1,  16, 2000, 2000, 100000 },
    {  926000000,  1000, true, 73,  8, 19,  1,  16, 2000, 2000, 100000 },
    {  927000000,  1000, true, 73,  8, 21,  1,  16, 2000, 2000, 100000 },
    {  923500000,  1000, true, 64,  9,  2,  2,  16, 2000, 2000, 100000 },
    {  924500000,  1000, true, 64, 10,  4,  2,  16, 2000, 2000, 100000 },
    {  925500000,  1000, true, 64,  9,  6,  2,  16, 2000, 2000, 100000 },
    {  926500000,  1000, true, 64, 10,  8,  2,  16, 2000, 2000, 100000 },
    {  924500000,  1000, true, 65, 11, 36,  4,  16, 2000, 2000, 100000 },
    {  925500000,  1000, true, 65, 12, 38,  4,  16, 2000, 2000, 100000 },
};

/** Channel list structure for Japan. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_JP = {
    .country_code = "JP",
    .num_channels = (sizeof(s1g_channels_JP)/sizeof(s1g_channels_JP[0])),
    .channels = s1g_channels_JP,
};

/** List of valid S1G channels for Kenya. */
static const struct mmwlan_s1g_channel s1g_channels_KE[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Kenya. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_KE = {
    .country_code = "KE",
    .num_channels = (sizeof(s1g_channels_KE)/sizeof(s1g_channels_KE[0])),
    .channels = s1g_channels_KE,
};

/** List of valid S1G channels for South Korea. */
static const struct mmwlan_s1g_channel s1g_channels_KR[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  918000000, 10000, false, 74, 14,  1,  1,   4, 50000, 0, 4000000 },
    {  919000000, 10000, false, 74, 14,  3,  1,   4, 50000, 0, 4000000 },
    {  920000000, 10000, false, 74, 14,  5,  1,   4, 50000, 0, 4000000 },
    {  921000000, 10000, false, 74, 14,  7,  1,   4, 50000, 0, 4000000 },
    {  922000000, 10000, false, 74, 14,  9,  1,  10, 50000, 0, 4000000 },
    {  923000000, 10000, false, 74, 14, 11,  1,  10, 50000, 0, 4000000 },
    {  918500000, 10000, false, 75, 15,  2,  2,   4, 50000, 0, 4000000 },
    {  920500000, 10000, false, 75, 15,  6,  2,   4, 50000, 0, 4000000 },
    {  922500000, 10000, false, 75, 15, 10,  2,  10, 50000, 0, 4000000 },
    {  921500000, 10000, false, 76, 16,  8,  4,   4, 50000, 0, 4000000 },
    {  926500000, 10000, false, 74, 14, 18,  1,  17, 264, 0, 220000 }, /* Warning: regulatory requirements may not be met */
    {  927500000, 10000, false, 74, 14, 20,  1,  17, 264, 0, 220000 }, /* Warning: regulatory requirements may not be met */
    {  928500000, 10000, false, 74, 14, 22,  1,  17, 264, 0, 220000 }, /* Warning: regulatory requirements may not be met */
    {  929500000, 10000, false, 74, 14, 24,  1,  17, 264, 0, 220000 }, /* Warning: regulatory requirements may not be met */
    {  927000000, 10000, false, 75, 15, 19,  2,  20, 264, 0, 220000 }, /* Warning: regulatory requirements may not be met */
    {  929000000, 10000, false, 75, 15, 23,  2,  20, 264, 0, 220000 }, /* Warning: regulatory requirements may not be met */
};

/** Channel list structure for South Korea. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_KR = {
    .country_code = "KR",
    .num_channels = (sizeof(s1g_channels_KR)/sizeof(s1g_channels_KR[0])),
    .channels = s1g_channels_KR,
};

/** List of valid S1G channels for Lithuania. */
static const struct mmwlan_s1g_channel s1g_channels_LT[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  916900000,   280, false, 77, 30, 31,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Lithuania. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_LT = {
    .country_code = "LT",
    .num_channels = (sizeof(s1g_channels_LT)/sizeof(s1g_channels_LT[0])),
    .channels = s1g_channels_LT,
};

/** List of valid S1G channels for Luxembourg. */
static const struct mmwlan_s1g_channel s1g_channels_LU[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Luxembourg. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_LU = {
    .country_code = "LU",
    .num_channels = (sizeof(s1g_channels_LU)/sizeof(s1g_channels_LU[0])),
    .channels = s1g_channels_LU,
};

/** List of valid S1G channels for Latvia. */
static const struct mmwlan_s1g_channel s1g_channels_LV[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Latvia. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_LV = {
    .country_code = "LV",
    .num_channels = (sizeof(s1g_channels_LV)/sizeof(s1g_channels_LV[0])),
    .channels = s1g_channels_LV,
};

/** List of valid S1G channels for Moldova. */
static const struct mmwlan_s1g_channel s1g_channels_MD[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  916900000,   280, false, 77, 30, 31,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Moldova. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_MD = {
    .country_code = "MD",
    .num_channels = (sizeof(s1g_channels_MD)/sizeof(s1g_channels_MD[0])),
    .channels = s1g_channels_MD,
};

/** List of valid S1G channels for North Macedonia. */
static const struct mmwlan_s1g_channel s1g_channels_MK[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  916900000,   280, false, 77, 30, 31,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for North Macedonia. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_MK = {
    .country_code = "MK",
    .num_channels = (sizeof(s1g_channels_MK)/sizeof(s1g_channels_MK[0])),
    .channels = s1g_channels_MK,
};

/** List of valid S1G channels for Malta. */
static const struct mmwlan_s1g_channel s1g_channels_MT[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Malta. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_MT = {
    .country_code = "MT",
    .num_channels = (sizeof(s1g_channels_MT)/sizeof(s1g_channels_MT[0])),
    .channels = s1g_channels_MT,
};

/** List of valid S1G channels for Netherlands. */
static const struct mmwlan_s1g_channel s1g_channels_NL[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Netherlands. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_NL = {
    .country_code = "NL",
    .num_channels = (sizeof(s1g_channels_NL)/sizeof(s1g_channels_NL[0])),
    .channels = s1g_channels_NL,
};

/** List of valid S1G channels for Norway. */
static const struct mmwlan_s1g_channel s1g_channels_NO[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Norway. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_NO = {
    .country_code = "NO",
    .num_channels = (sizeof(s1g_channels_NO)/sizeof(s1g_channels_NO[0])),
    .channels = s1g_channels_NO,
};

/** List of valid S1G channels for New Zealand. */
static const struct mmwlan_s1g_channel s1g_channels_NZ[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  915500000, 10000, false, 68, 26, 27,  1,  30, 0, 0, 0 },
    {  916500000, 10000, false, 68, 26, 29,  1,  30, 0, 0, 0 },
    {  917500000, 10000, false, 68, 26, 31,  1,  30, 0, 0, 0 },
    {  918500000, 10000, false, 68, 26, 33,  1,  30, 0, 0, 0 },
    {  919500000, 10000, false, 68, 26, 35,  1,  30, 0, 0, 0 },
    {  920500000, 10000, false, 68, 26, 37,  1,  36, 0, 0, 0 },
    {  921500000, 10000, false, 68, 26, 39,  1,  36, 0, 0, 0 },
    {  922500000, 10000, false, 68, 26, 41,  1,  36, 0, 0, 0 },
    {  923500000, 10000, false, 68, 26, 43,  1,  36, 0, 0, 0 },
    {  924500000, 10000, false, 68, 26, 45,  1,  36, 0, 0, 0 },
    {  925500000, 10000, false, 68, 26, 47,  1,  36, 0, 0, 0 },
    {  926500000, 10000, false, 68, 26, 49,  1,  36, 0, 0, 0 },
    {  927500000, 10000, false, 68, 26, 51,  1,  36, 0, 0, 0 },
    {  917000000, 10000, false, 69, 27, 30,  2,  30, 0, 0, 0 },
    {  919000000, 10000, false, 69, 27, 34,  2,  30, 0, 0, 0 },
    {  921000000, 10000, false, 69, 27, 38,  2,  36, 0, 0, 0 },
    {  923000000, 10000, false, 69, 27, 42,  2,  36, 0, 0, 0 },
    {  925000000, 10000, false, 69, 27, 46,  2,  36, 0, 0, 0 },
    {  927000000, 10000, false, 69, 27, 50,  2,  36, 0, 0, 0 },
    {  918000000, 10000, false, 70, 28, 32,  4,  30, 0, 0, 0 },
    {  922000000, 10000, false, 70, 28, 40,  4,  36, 0, 0, 0 },
    {  926000000, 10000, false, 70, 28, 48,  4,  36, 0, 0, 0 },
    {  924000000, 10000, false, 71, 29, 44,  8,  36, 0, 0, 0 },
};

/** Channel list structure for New Zealand. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_NZ = {
    .country_code = "NZ",
    .num_channels = (sizeof(s1g_channels_NZ)/sizeof(s1g_channels_NZ[0])),
    .channels = s1g_channels_NZ,
};

/** List of valid S1G channels for Poland. */
static const struct mmwlan_s1g_channel s1g_channels_PL[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Poland. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_PL = {
    .country_code = "PL",
    .num_channels = (sizeof(s1g_channels_PL)/sizeof(s1g_channels_PL[0])),
    .channels = s1g_channels_PL,
};

/** List of valid S1G channels for Portugal. */
static const struct mmwlan_s1g_channel s1g_channels_PT[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Portugal. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_PT = {
    .country_code = "PT",
    .num_channels = (sizeof(s1g_channels_PT)/sizeof(s1g_channels_PT[0])),
    .channels = s1g_channels_PT,
};

/** List of valid S1G channels for Romania. */
static const struct mmwlan_s1g_channel s1g_channels_RO[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Romania. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_RO = {
    .country_code = "RO",
    .num_channels = (sizeof(s1g_channels_RO)/sizeof(s1g_channels_RO[0])),
    .channels = s1g_channels_RO,
};

/** List of valid S1G channels for Sweden. */
static const struct mmwlan_s1g_channel s1g_channels_SE[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  916900000,   280, false, 77, 30, 31,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Sweden. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_SE = {
    .country_code = "SE",
    .num_channels = (sizeof(s1g_channels_SE)/sizeof(s1g_channels_SE[0])),
    .channels = s1g_channels_SE,
};

/** List of valid S1G channels for Singapore. */
static const struct mmwlan_s1g_channel s1g_channels_SG[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  866500000,   277, false, 66, 17,  7,  1,  29, 100000, 0, 1000000 },
    {  867500000,   277, false, 66, 17,  9,  1,  29, 100000, 0, 1000000 },
    {  868500000,   277, false, 66, 17, 11,  1,  29, 100000, 0, 1000000 },
    {  868000000,   277, false, 67, 19, 10,  2,  29, 100000, 0, 1000000 },
    {  920500000, 10000, false, 68, 18, 37,  1,  22, 0, 0, 0 },
    {  921500000, 10000, false, 68, 18, 39,  1,  22, 0, 0, 0 },
    {  922500000, 10000, false, 68, 18, 41,  1,  22, 0, 0, 0 },
    {  923500000, 10000, false, 68, 18, 43,  1,  22, 0, 0, 0 },
    {  924500000, 10000, false, 68, 18, 45,  1,  22, 0, 0, 0 },
    {  921000000, 10000, false, 69, 20, 38,  2,  22, 0, 0, 0 },
    {  923000000, 10000, false, 69, 20, 42,  2,  22, 0, 0, 0 },
    {  922000000, 10000, false, 70, 21, 40,  4,  22, 0, 0, 0 },
};

/** Channel list structure for Singapore. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_SG = {
    .country_code = "SG",
    .num_channels = (sizeof(s1g_channels_SG)/sizeof(s1g_channels_SG[0])),
    .channels = s1g_channels_SG,
};

/** List of valid S1G channels for Slovenia. */
static const struct mmwlan_s1g_channel s1g_channels_SI[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  916900000,   280, false, 77, 30, 31,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Slovenia. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_SI = {
    .country_code = "SI",
    .num_channels = (sizeof(s1g_channels_SI)/sizeof(s1g_channels_SI[0])),
    .channels = s1g_channels_SI,
};

/** List of valid S1G channels for Slovakia. */
static const struct mmwlan_s1g_channel s1g_channels_SK[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Slovakia. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_SK = {
    .country_code = "SK",
    .num_channels = (sizeof(s1g_channels_SK)/sizeof(s1g_channels_SK[0])),
    .channels = s1g_channels_SK,
};

/** List of valid S1G channels for Turkey. */
static const struct mmwlan_s1g_channel s1g_channels_TR[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for Turkey. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_TR = {
    .country_code = "TR",
    .num_channels = (sizeof(s1g_channels_TR)/sizeof(s1g_channels_TR[0])),
    .channels = s1g_channels_TR,
};

/** List of valid S1G channels for United Kingdom. */
static const struct mmwlan_s1g_channel s1g_channels_UK[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  863500000,   280, false, 66,  6,  1,  1,  16, 0, 0, 0 },
    {  864500000,   280, false, 66,  6,  3,  1,  16, 0, 0, 0 },
    {  865500000,   280, false, 66,  6,  5,  1,  16, 0, 0, 0 },
    {  866500000,   280, false, 66,  6,  7,  1,  16, 0, 0, 0 },
    {  867500000,   280, false, 66,  6,  9,  1,  16, 0, 0, 0 },
    {  917900000,   280, false, 77, 30, 33,  1,  16, 0, 0, 0 },
    {  918900000,   280, false, 77, 30, 35,  1,  16, 0, 0, 0 },
};

/** Channel list structure for United Kingdom. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_UK = {
    .country_code = "UK",
    .num_channels = (sizeof(s1g_channels_UK)/sizeof(s1g_channels_UK[0])),
    .channels = s1g_channels_UK,
};

/** List of valid S1G channels for USA. */
static const struct mmwlan_s1g_channel s1g_channels_US[] = {
    /* Ctr Freq (Hz), Duty Cycle (%/100), Omit Control Response, Global Op Class, S1G Op Class, S1G Chan #, Op BW, Max Tx EIRP (dBm), Min Packet Spacing Window (microsec), airtime_min (microsec), airtime_max (microsec) */
    {  902500000, 10000, false, 68,  1,  1,  1,  36, 0, 0, 0 }, /* Warning: regulatory requirements may not be met */
    {  903500000, 10000, false, 68,  1,  3,  1,  36, 0, 0, 0 },
    {  904500000, 10000, false, 68,  1,  5,  1,  36, 0, 0, 0 },
    {  905500000, 10000, false, 68,  1,  7,  1,  36, 0, 0, 0 },
    {  906500000, 10000, false, 68,  1,  9,  1,  36, 0, 0, 0 },
    {  907500000, 10000, false, 68,  1, 11,  1,  36, 0, 0, 0 },
    {  908500000, 10000, false, 68,  1, 13,  1,  36, 0, 0, 0 },
    {  909500000, 10000, false, 68,  1, 15,  1,  36, 0, 0, 0 },
    {  910500000, 10000, false, 68,  1, 17,  1,  36, 0, 0, 0 },
    {  911500000, 10000, false, 68,  1, 19,  1,  36, 0, 0, 0 },
    {  912500000, 10000, false, 68,  1, 21,  1,  36, 0, 0, 0 },
    {  913500000, 10000, false, 68,  1, 23,  1,  36, 0, 0, 0 },
    {  914500000, 10000, false, 68,  1, 25,  1,  36, 0, 0, 0 },
    {  915500000, 10000, false, 68,  1, 27,  1,  36, 0, 0, 0 },
    {  916500000, 10000, false, 68,  1, 29,  1,  36, 0, 0, 0 },
    {  917500000, 10000, false, 68,  1, 31,  1,  36, 0, 0, 0 },
    {  918500000, 10000, false, 68,  1, 33,  1,  36, 0, 0, 0 },
    {  919500000, 10000, false, 68,  1, 35,  1,  36, 0, 0, 0 },
    {  920500000, 10000, false, 68,  1, 37,  1,  36, 0, 0, 0 },
    {  921500000, 10000, false, 68,  1, 39,  1,  36, 0, 0, 0 },
    {  922500000, 10000, false, 68,  1, 41,  1,  36, 0, 0, 0 },
    {  923500000, 10000, false, 68,  1, 43,  1,  36, 0, 0, 0 },
    {  924500000, 10000, false, 68,  1, 45,  1,  36, 0, 0, 0 },
    {  925500000, 10000, false, 68,  1, 47,  1,  36, 0, 0, 0 },
    {  926500000, 10000, false, 68,  1, 49,  1,  36, 0, 0, 0 },
    {  927500000, 10000, false, 68,  1, 51,  1,  36, 0, 0, 0 },
    {  903000000, 10000, false, 69,  2,  2,  2,  36, 0, 0, 0 }, /* Warning: regulatory requirements may not be met */
    {  905000000, 10000, false, 69,  2,  6,  2,  36, 0, 0, 0 },
    {  907000000, 10000, false, 69,  2, 10,  2,  36, 0, 0, 0 },
    {  909000000, 10000, false, 69,  2, 14,  2,  36, 0, 0, 0 },
    {  911000000, 10000, false, 69,  2, 18,  2,  36, 0, 0, 0 },
    {  913000000, 10000, false, 69,  2, 22,  2,  36, 0, 0, 0 },
    {  915000000, 10000, false, 69,  2, 26,  2,  36, 0, 0, 0 },
    {  917000000, 10000, false, 69,  2, 30,  2,  36, 0, 0, 0 },
    {  919000000, 10000, false, 69,  2, 34,  2,  36, 0, 0, 0 },
    {  921000000, 10000, false, 69,  2, 38,  2,  36, 0, 0, 0 },
    {  923000000, 10000, false, 69,  2, 42,  2,  36, 0, 0, 0 },
    {  925000000, 10000, false, 69,  2, 46,  2,  36, 0, 0, 0 },
    {  927000000, 10000, false, 69,  2, 50,  2,  36, 0, 0, 0 },
    {  906000000, 10000, false, 70,  3,  8,  4,  36, 0, 0, 0 },
    {  910000000, 10000, false, 70,  3, 16,  4,  36, 0, 0, 0 },
    {  914000000, 10000, false, 70,  3, 24,  4,  36, 0, 0, 0 },
    {  918000000, 10000, false, 70,  3, 32,  4,  36, 0, 0, 0 },
    {  922000000, 10000, false, 70,  3, 40,  4,  36, 0, 0, 0 },
    {  926000000, 10000, false, 70,  3, 48,  4,  36, 0, 0, 0 },
    {  908000000, 10000, false, 71,  4, 12,  8,  36, 0, 0, 0 },
    {  916000000, 10000, false, 71,  4, 28,  8,  36, 0, 0, 0 },
    {  924000000, 10000, false, 71,  4, 44,  8,  36, 0, 0, 0 },
};

/** Channel list structure for USA. */
static const struct mmwlan_s1g_channel_list s1g_channel_list_US = {
    .country_code = "US",
    .num_channels = (sizeof(s1g_channels_US)/sizeof(s1g_channels_US[0])),
    .channels = s1g_channels_US,
};

/** Array of all channel list structs used for the regulatory database. */

static const struct mmwlan_s1g_channel_list *regulatory_db_domains[] = {
    &s1g_channel_list_AL,
    &s1g_channel_list_AR,
    &s1g_channel_list_AT,
    &s1g_channel_list_AU,
    &s1g_channel_list_BA,
    &s1g_channel_list_BE,
    &s1g_channel_list_BG,
    &s1g_channel_list_BR,
    &s1g_channel_list_CA,
    &s1g_channel_list_CH,
    &s1g_channel_list_CY,
    &s1g_channel_list_CZ,
    &s1g_channel_list_DD,
    &s1g_channel_list_DK,
    &s1g_channel_list_EE,
    &s1g_channel_list_ES,
    &s1g_channel_list_EU,
    &s1g_channel_list_FI,
    &s1g_channel_list_FL,
    &s1g_channel_list_FR,
    &s1g_channel_list_GR,
    &s1g_channel_list_HR,
    &s1g_channel_list_HU,
    &s1g_channel_list_IE,
    &s1g_channel_list_IN,
    &s1g_channel_list_IS,
    &s1g_channel_list_IT,
    &s1g_channel_list_JP,
    &s1g_channel_list_KE,
    &s1g_channel_list_KR,
    &s1g_channel_list_LT,
    &s1g_channel_list_LU,
    &s1g_channel_list_LV,
    &s1g_channel_list_MD,
    &s1g_channel_list_MK,
    &s1g_channel_list_MT,
    &s1g_channel_list_NL,
    &s1g_channel_list_NO,
    &s1g_channel_list_NZ,
    &s1g_channel_list_PL,
    &s1g_channel_list_PT,
    &s1g_channel_list_RO,
    &s1g_channel_list_SE,
    &s1g_channel_list_SG,
    &s1g_channel_list_SI,
    &s1g_channel_list_SK,
    &s1g_channel_list_TR,
    &s1g_channel_list_UK,
    &s1g_channel_list_US,
};

/** Regulatory database. */
static const struct mmwlan_regulatory_db regulatory_db = {
    .num_domains = (sizeof(regulatory_db_domains)/sizeof(regulatory_db_domains[0])),
    .domains = regulatory_db_domains,
};

/**
 * Get a pointer to regulatory_db. This function isn't strictly necessary, since regulatory_db
 * can be accessed directly, but will prevent the compiler from generated warnings about
 * regulatory_db being unused.
 *
 * @return Reference to the regulatory database
 */
static inline const struct mmwlan_regulatory_db *get_regulatory_db(void)
{
    return &regulatory_db;
}

#endif
/** \} */