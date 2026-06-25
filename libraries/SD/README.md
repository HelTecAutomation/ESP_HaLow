# SD library

This library provides the integration of ESP32 and SD (Secure Digital) and MMC (Multi Media Card) cards without additional modules. This library is using SPI to interface with the cards. Please note that SPI mode is slower than the intended SD or MMC mode, however, provides more flexibility as the SPI module is available on all ESP SoCs and can be routed to any GPIO through GPIO matrix.

## Sample wiring diagram:

![Connections](http://i.imgur.com/4CoXOuR.png)

For other SD formats:

![Other SD card formats](https://upload.wikimedia.org/wikipedia/commons/thumb/a/ab/MMC-SD-miniSD-microSD-Color-Numbers-Names.gif/330px-MMC-SD-miniSD-microSD-Color-Numbers-Names.gif)

Image source: [Wikipedia](https://upload.wikimedia.org/wikipedia/commons/thumb/a/ab/MMC-SD-miniSD-microSD-Color-Numbers-Names.gif/330px-MMC-SD-miniSD-microSD-Color-Numbers-Names.gif)

> **Warning**
Some ESP32 modules have different pin outs!

## Default SPI pins:
Note that SPI pins can be configured by using `SPI.begin(sck, miso, mosi, cs);` alternatively, you can change only the CS pin with `SD.begin(CSpin)`

| SPI Pin Name | ESP8266 | ESP32 | ESP32-S2 | ESP32-C3 | ESP32-S3 |
|--------------|---------|-------|----------|----------|----------|
| CS (SS)      | GPIO15  | GPIO5 | GPIO5    | GPIO13   | GPIO13   |
| DI (MOSI)    | GPIO13  | GPIO23| GPIO24   | GPIO14   | GPIO14   |
| DO (MISO)    | GPIO12  | GPIO19| GPIO25   | GPIO15   | GPIO15   |
| SCK (SCLK)   | GPIO14  | GPIO18| GPIO19   | GPIO16   | GPIO16   |

## Heads-up: a serial monitor can make a *present* SD card look missing

On boards that use the standard ESP32 **DTR/RTS auto-reset** circuit, opening a
serial monitor toggles DTR/RTS and **warm-resets the MCU** — but this does *not*
power-cycle the SD card. A card that mounted fine on the real cold boot can then
fail to re-initialize after that warm reset and report `Card Mount Failed`,
making it look like no card is inserted even though one is.

Observed with the **Arduino IDE 2.3.10** Serial Monitor, but it applies to any
tool that asserts DTR/RTS when it opens the port.

**Workarounds**

- **Power-cycle** the board (fully unplug USB for a few seconds) before trusting a
  "card missing" result — pressing reset or re-flashing does *not* power-cycle the
  card.
- Or watch the port with a tool that does **not** toggle DTR/RTS. A minimal
  [pyserial](https://pypi.org/project/pyserial/) reader works well:

  ```python
  import serial, sys
  p = serial.Serial()
  p.port = "/dev/cu.usbserial-0001"   # your port
  p.baudrate = 115200
  p.dtr = False                       # hold the reset line inactive...
  p.rts = False                       # ...so opening the port does NOT reset the board
  p.open()
  while True:
      data = p.read(p.in_waiting or 1)
      if data:
          sys.stdout.write(data.decode("utf-8", "replace")); sys.stdout.flush()
  ```

## FAQ:

**Do I need any additional modules**, like **the **Arduino**** SD module**?**

No, just wire your SD card directly to ESP32.

Tip: If you are using a microSD card and have a spare adapter to full-sized SD, you can solder Dupont pins on the adapter.


**What is the difference between SD and SD_MMC libraries?**

SD runs on SPI, and SD_MMC uses the SDMMC hardware bus on the ESP32.
The SPI uses 4 communication pins + 2 power connections and operates on up to 80MHz. The SPI option offers flexibility on pin connection because the data connections can be routed through GPIO matrix to any data pin.
SD-SPI speed is approximately half of the SD-MMC even when used on 1-bit line.
You can read more about SD SPI in the [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/sdspi_host.html)

SD_MMC is supported only by ESP32 and ESP32-S3 and can be connected only to dedicated pins. SD_MMC allows to use of 1, 4 or 8 data pins + 2 additional communication pins and 2 power pins. The data pins need to be pulled up externally.
You can read more about SD_MMC in the [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/sdmmc_host.html)
1-bit: SD_MMC_ speed is approximately two-times faster than SPI mode
4-bit: SD_MMC speed is approximately three-times faster than SPI mode.
