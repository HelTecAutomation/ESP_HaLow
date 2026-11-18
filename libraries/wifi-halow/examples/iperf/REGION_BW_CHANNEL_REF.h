/*
 * S1G region / bandwidth / channel reference.
 *
 * Generated from src/mmregdb.c. Frequencies are center frequencies.
 * Occupied ranges are center frequency +/- bandwidth/2.
 *
 * Country code list
 *
 * | Country Code | Country |
 * | ------------ | ------- |
 * | AU | Australia |
 * | CA | Canada |
 * | EU | User assigned EU |
 * | GB | United Kingdom |
 * | IN | India |
 * | JP | Japan |
 * | KR | South Korea |
 * | NZ | New Zealand |
 * | US | USA |
 *
 * Australia variants:
 *   AU_2020 - IEEE 802.11-2020 Australia channel set (23 channels)
 *   AU_2024 - IEEE 802.11-2024 Australia channel set (23 channels), also the default AU list
 *   AU_revmf - IEEE 802.11-REVMF Australia channel set (26 channels; AU_2024 plus extra 4M/8M)
 *
 * Each region lists channels in 1M, 2M, 4M, 8M bandwidth order.
 *
 *   AU_2020 (Australia, 802.11-2020) - occupied: 915.0-928.0 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |    27    |   915.5MHz
 *   1M  |    29    |   916.5MHz
 *   1M  |    31    |   917.5MHz
 *   1M  |    33    |   918.5MHz
 *   1M  |    35    |   919.5MHz
 *   1M  |    37    |   920.5MHz
 *   1M  |    39    |   921.5MHz
 *   1M  |    41    |   922.5MHz
 *   1M  |    43    |   923.5MHz
 *   1M  |    45    |   924.5MHz
 *   1M  |    47    |   925.5MHz
 *   1M  |    49    |   926.5MHz
 *   1M  |    51    |   927.5MHz
 *   2M  |    30    |   917.0MHz
 *   2M  |    34    |   919.0MHz
 *   2M  |    38    |   921.0MHz
 *   2M  |    42    |   923.0MHz
 *   2M  |    46    |   925.0MHz
 *   2M  |    50    |   927.0MHz
 *   4M  |    32    |   918.0MHz
 *   4M  |    40    |   922.0MHz
 *   4M  |    48    |   926.0MHz
 *   8M  |    44    |   924.0MHz
 *
 *   AU_2024 (Australia, 802.11-2024) - occupied: 915.5-927.5 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |    28    |   916.0MHz
 *   1M  |    30    |   917.0MHz
 *   1M  |    32    |   918.0MHz
 *   1M  |    34    |   919.0MHz
 *   1M  |    36    |   920.0MHz
 *   1M  |    38    |   921.0MHz
 *   1M  |    40    |   922.0MHz
 *   1M  |    42    |   923.0MHz
 *   1M  |    44    |   924.0MHz
 *   1M  |    46    |   925.0MHz
 *   1M  |    48    |   926.0MHz
 *   1M  |    50    |   927.0MHz
 *   2M  |    29    |   916.5MHz
 *   2M  |    33    |   918.5MHz
 *   2M  |    37    |   920.5MHz
 *   2M  |    41    |   922.5MHz
 *   2M  |    45    |   924.5MHz
 *   2M  |    49    |   926.5MHz
 *   4M  |    31    |   917.5MHz
 *   4M  |    39    |   921.5MHz
 *   4M  |    47    |   925.5MHz
 *   8M  |    35    |   919.5MHz
 *   8M  |    43    |   923.5MHz
 *
 *   AU_revmf (Australia, 802.11-REVMF) - occupied: 915.5-927.5 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |    28    |   916.0MHz
 *   1M  |    30    |   917.0MHz
 *   1M  |    32    |   918.0MHz
 *   1M  |    34    |   919.0MHz
 *   1M  |    36    |   920.0MHz
 *   1M  |    38    |   921.0MHz
 *   1M  |    40    |   922.0MHz
 *   1M  |    42    |   923.0MHz
 *   1M  |    44    |   924.0MHz
 *   1M  |    46    |   925.0MHz
 *   1M  |    48    |   926.0MHz
 *   1M  |    50    |   927.0MHz
 *   2M  |    29    |   916.5MHz
 *   2M  |    33    |   918.5MHz
 *   2M  |    37    |   920.5MHz
 *   2M  |    41    |   922.5MHz
 *   2M  |    45    |   924.5MHz
 *   2M  |    49    |   926.5MHz
 *   4M  |    31    |   917.5MHz
 *   4M  |    39    |   921.5MHz
 *   4M  |    47    |   925.5MHz
 *   4M  |    51    |   919.5MHz
 *   4M  |    59    |   923.5MHz
 *   8M  |    35    |   919.5MHz
 *   8M  |    43    |   923.5MHz
 *   8M  |    55    |   921.5MHz
 *
 *   CA (Canada) - occupied: 902.0-928.0 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |     1    |   902.5MHz
 *   1M  |     3    |   903.5MHz
 *   1M  |     5    |   904.5MHz
 *   1M  |     7    |   905.5MHz
 *   1M  |     9    |   906.5MHz
 *   1M  |    11    |   907.5MHz
 *   1M  |    13    |   908.5MHz
 *   1M  |    15    |   909.5MHz
 *   1M  |    17    |   910.5MHz
 *   1M  |    19    |   911.5MHz
 *   1M  |    21    |   912.5MHz
 *   1M  |    23    |   913.5MHz
 *   1M  |    25    |   914.5MHz
 *   1M  |    27    |   915.5MHz
 *   1M  |    29    |   916.5MHz
 *   1M  |    31    |   917.5MHz
 *   1M  |    33    |   918.5MHz
 *   1M  |    35    |   919.5MHz
 *   1M  |    37    |   920.5MHz
 *   1M  |    39    |   921.5MHz
 *   1M  |    41    |   922.5MHz
 *   1M  |    43    |   923.5MHz
 *   1M  |    45    |   924.5MHz
 *   1M  |    47    |   925.5MHz
 *   1M  |    49    |   926.5MHz
 *   1M  |    51    |   927.5MHz
 *   2M  |     2    |   903.0MHz
 *   2M  |     6    |   905.0MHz
 *   2M  |    10    |   907.0MHz
 *   2M  |    14    |   909.0MHz
 *   2M  |    18    |   911.0MHz
 *   2M  |    22    |   913.0MHz
 *   2M  |    26    |   915.0MHz
 *   2M  |    30    |   917.0MHz
 *   2M  |    34    |   919.0MHz
 *   2M  |    38    |   921.0MHz
 *   2M  |    42    |   923.0MHz
 *   2M  |    46    |   925.0MHz
 *   2M  |    50    |   927.0MHz
 *   4M  |     8    |   906.0MHz
 *   4M  |    16    |   910.0MHz
 *   4M  |    24    |   914.0MHz
 *   4M  |    32    |   918.0MHz
 *   4M  |    40    |   922.0MHz
 *   4M  |    48    |   926.0MHz
 *   8M  |    12    |   908.0MHz
 *   8M  |    28    |   916.0MHz
 *   8M  |    44    |   924.0MHz
 *
 *   EU (User assigned EU) - occupied: 863.0-868.0 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |     1    |   863.5MHz
 *   1M  |     3    |   864.5MHz
 *   1M  |     5    |   865.5MHz
 *   1M  |     7    |   866.5MHz
 *   1M  |     9    |   867.5MHz
 *   2M  |     2    |   864.0MHz
 *   2M  |     6    |   866.0MHz
 *
 *   GB (United Kingdom) - occupied: 863.0-868.0, 917.4-919.4 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |     1    |   863.5MHz
 *   1M  |     3    |   864.5MHz
 *   1M  |     5    |   865.5MHz
 *   1M  |     7    |   866.5MHz
 *   1M  |     9    |   867.5MHz
 *   1M  |    33    |   917.9MHz
 *   1M  |    35    |   918.9MHz
 *   2M  |     2    |   864.0MHz
 *   2M  |     6    |   866.0MHz
 *
 *   IN (India) - occupied: 865.0-868.0 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |     5    |   865.5MHz
 *   1M  |     7    |   866.5MHz
 *   1M  |     9    |   867.5MHz
 *
 *   JP (Japan) - occupied: 920.5-921.5, 922.5-927.5 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |     9    |   921.0MHz
 *   1M  |    13    |   923.0MHz
 *   1M  |    15    |   924.0MHz
 *   1M  |    17    |   925.0MHz
 *   1M  |    19    |   926.0MHz
 *   1M  |    21    |   927.0MHz
 *   2M  |     2    |   923.5MHz
 *   2M  |     4    |   924.5MHz
 *   2M  |     6    |   925.5MHz
 *   2M  |     8    |   926.5MHz
 *   4M  |    36    |   924.5MHz
 *   4M  |    38    |   925.5MHz
 *
 *   KR (South Korea) - occupied: 917.5-923.5, 926.0-930.0 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |     1    |   918.0MHz
 *   1M  |     3    |   919.0MHz
 *   1M  |     5    |   920.0MHz
 *   1M  |     7    |   921.0MHz
 *   1M  |     9    |   922.0MHz
 *   1M  |    11    |   923.0MHz
 *   1M  |    18    |   926.5MHz
 *   1M  |    20    |   927.5MHz
 *   1M  |    22    |   928.5MHz
 *   1M  |    24    |   929.5MHz
 *   2M  |     2    |   918.5MHz
 *   2M  |     6    |   920.5MHz
 *   2M  |    10    |   922.5MHz
 *   2M  |    19    |   927.0MHz
 *   2M  |    23    |   929.0MHz
 *   4M  |     8    |   921.5MHz
 *
 *   NZ (New Zealand) - occupied: 915.0-928.0 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |    27    |   915.5MHz
 *   1M  |    29    |   916.5MHz
 *   1M  |    31    |   917.5MHz
 *   1M  |    33    |   918.5MHz
 *   1M  |    35    |   919.5MHz
 *   1M  |    37    |   920.5MHz
 *   1M  |    39    |   921.5MHz
 *   1M  |    41    |   922.5MHz
 *   1M  |    43    |   923.5MHz
 *   1M  |    45    |   924.5MHz
 *   1M  |    47    |   925.5MHz
 *   1M  |    49    |   926.5MHz
 *   1M  |    51    |   927.5MHz
 *   2M  |    30    |   917.0MHz
 *   2M  |    34    |   919.0MHz
 *   2M  |    38    |   921.0MHz
 *   2M  |    42    |   923.0MHz
 *   2M  |    46    |   925.0MHz
 *   2M  |    50    |   927.0MHz
 *   4M  |    32    |   918.0MHz
 *   4M  |    40    |   922.0MHz
 *   4M  |    48    |   926.0MHz
 *   8M  |    44    |   924.0MHz
 *
 *   US (USA) - occupied: 902.0-928.0 MHz
 *   BW  | channels |    freq
 *   --- | -------- | ----------
 *   1M  |     1    |   902.5MHz
 *   1M  |     3    |   903.5MHz
 *   1M  |     5    |   904.5MHz
 *   1M  |     7    |   905.5MHz
 *   1M  |     9    |   906.5MHz
 *   1M  |    11    |   907.5MHz
 *   1M  |    13    |   908.5MHz
 *   1M  |    15    |   909.5MHz
 *   1M  |    17    |   910.5MHz
 *   1M  |    19    |   911.5MHz
 *   1M  |    21    |   912.5MHz
 *   1M  |    23    |   913.5MHz
 *   1M  |    25    |   914.5MHz
 *   1M  |    27    |   915.5MHz
 *   1M  |    29    |   916.5MHz
 *   1M  |    31    |   917.5MHz
 *   1M  |    33    |   918.5MHz
 *   1M  |    35    |   919.5MHz
 *   1M  |    37    |   920.5MHz
 *   1M  |    39    |   921.5MHz
 *   1M  |    41    |   922.5MHz
 *   1M  |    43    |   923.5MHz
 *   1M  |    45    |   924.5MHz
 *   1M  |    47    |   925.5MHz
 *   1M  |    49    |   926.5MHz
 *   1M  |    51    |   927.5MHz
 *   2M  |     2    |   903.0MHz
 *   2M  |     6    |   905.0MHz
 *   2M  |    10    |   907.0MHz
 *   2M  |    14    |   909.0MHz
 *   2M  |    18    |   911.0MHz
 *   2M  |    22    |   913.0MHz
 *   2M  |    26    |   915.0MHz
 *   2M  |    30    |   917.0MHz
 *   2M  |    34    |   919.0MHz
 *   2M  |    38    |   921.0MHz
 *   2M  |    42    |   923.0MHz
 *   2M  |    46    |   925.0MHz
 *   2M  |    50    |   927.0MHz
 *   4M  |     8    |   906.0MHz
 *   4M  |    16    |   910.0MHz
 *   4M  |    24    |   914.0MHz
 *   4M  |    32    |   918.0MHz
 *   4M  |    40    |   922.0MHz
 *   4M  |    48    |   926.0MHz
 *   8M  |    12    |   908.0MHz
 *   8M  |    28    |   916.0MHz
 *   8M  |    44    |   924.0MHz
 *
 */
