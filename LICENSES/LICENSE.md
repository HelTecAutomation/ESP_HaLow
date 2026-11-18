# Heltec HaLow Arduino Library License

Copyright (c) 2024-2026 Heltec Automation

This repository contains both Heltec-authored code and third-party components.
The license below applies only to Heltec-authored files unless a file states
otherwise. Third-party components remain under their original licenses.

## Heltec-authored code

SPDX-License-Identifier: MIT

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Arduino-ESP32

Arduino-ESP32 is licensed under LGPL-2.1-or-later.

- Full license text: `LICENSES/LGPL-2.1-or-later.txt`
- Upstream: https://github.com/espressif/arduino-esp32
- Arduino-ESP32 copyright and license notices are retained.

This distribution includes binary libraries that depend on Arduino-ESP32.
Corresponding Arduino-ESP32 source code and build scripts are available from
the upstream project and the build scripts provided with this library. Users
may replace or relink the LGPL-licensed Arduino-ESP32 components with modified
versions.

## Morse Micro components

Morse Micro binary components are licensed under the Morse Micro Binary
Distribution License Agreement (BDL).

- Full BDL text: `LICENSES/LicenseRef-MorseMicroBDL.txt`
- Morse Micro copyright and license notices are retained.
- The Morse Micro binary components are licensed only for use with hardware
  that includes a Morse Micro Wi-Fi HaLow chip.
- The Morse Micro binary components must be distributed complete and
  unmodified, as provided by Morse Micro.
- Morse Micro firmware and BCF binaries are not distributed with this library.

Morse Micro binary components included in this distribution include:


The BDL export restrictions apply to the Morse Micro binary components. Export,
re-export or transfer to Cuba, Iran, North Korea, Sudan and Syria is prohibited.

Some Morse Micro source components are built from source and carry their own
per-file SPDX licenses:

These source-built components are licensed under Apache-2.0 or BSD-3-Clause
as marked in their source files, not under the Morse Micro BDL. If a binary
artifact contains Morse Micro firmware or BCF blobs, those blobs remain subject
to the Morse Micro BDL. This library does not distribute Morse Micro firmware
or BCF binaries.

## Other third-party components

Other third-party components remain under their own licenses:

- Apache-2.0: `LICENSES/Apache-2.0.txt`
- BSD-3-Clause: `LICENSES/BSD-3-Clause.txt`
- MIT: `LICENSES/MIT.txt`

Their copyright and SPDX notices are retained in the corresponding source
files and upstream projects.

## Trademarks

Heltec, Morse Micro, Arduino and Espressif names and logos are trademarks of
their respective owners. This license does not grant trademark rights.

## No warranty

The Software is provided without warranty of any kind, as stated in the MIT
license above and the applicable third-party licenses.
