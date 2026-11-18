# ESP32S3 WiFi HaLow Development Framework
![GitHub License](https://github.com/HelTecAutomation/ESP_HaLow) ![GitHub Release](https://github.com/HelTecAutomation/ESP_HaLow) ![GitHub Release Date - Published_At](https://github.com/HelTecAutomation/ESP_HaLow) ![GitHub Downloads (all assets, all releases)](https://github.com/HelTecAutomation/ESP_HaLow/total)

English | [简体中文](#简体中文)

*Some libraries of [Morse Micro](https://www.morsemicro.com/) Wi-Fi HaLow conflict with the latest ESP32S3 development environment, so the base code of ESP32S3 is stripped out and this development environment is established.*

Arduino source codes and tool chain for ESP32&Wi-Fi_HaLow series made by Heltec Automation, contains some basic code, Wi-Fi, BlueTooth, Wi-Fi HaLow and other related examples.

## Contents

  - [Features](#features)
  - [Example overview](#example-overview)
  - [Performance](#performance)
  - [Instructions](#Instructions)
  - [Installation Instructions](#Installation-Instructions)
  - [Decoding exceptions](#Decoding-exceptions)
  - [Issue/Bug report template](#Issue-Bug-report-template)
  - [Contact us](#Contact-us)

## Features

- Wi-Fi HaLow STA, AP, 802.11s mesh gateway and mesh point modes.
- Configurable region, channel and bandwidth. The region/bandwidth/channel reference is in `examples/HalowAP/REGION_BW_CHANNEL_REF.h`.
- STA and mesh point support DHCP or static IP.
- AP and mesh gateway support an embedded DHCP server.
- AP mode supports up to 8 STAs; mesh mode supports up to 8 peers.
- 802.11s mesh peer whitelist, forwarding statistics and per-peer link statistics.
- RSSI, noise, bandwidth and frequency reporting for STA and mesh links.
- NAPT between Wi-Fi and HaLow:
  - Wi-Fi STA uplink to HaLow AP
  - HaLow STA uplink to Wi-Fi AP
  - Wi-Fi STA uplink to mesh gateway
  - Mesh point uplink to Wi-Fi AP
- TCP/UDP throughput tests with fixed configuration or serial commands.
- Arduino event API for STA, AP and mesh state changes, including mesh peer connect/disconnect events.
- HTTP/HTTPS client, web server, camera web server, MQTT and HTTP/Web OTA examples.
- UART-to-TCP/UDP bridge and low-power UDP client examples.
- HaLow scan with RSSI, noise and channel information.

## Example overview

| Example | Description |
| --- | --- |
| `HalowSTA` | Connect to a HaLow AP as STA |
| `HalowClient` / `HalowClientStaticIP` | TCP/UDP client with DHCP or static IP |
| `HalowAP` | Create a HaLow AP with optional DHCP server |
| `HaLowMesh` | 802.11s mesh gateway/point, DHCP/static IP, peer whitelist and peer statistics |
| `HalowScan` | Scan HaLow APs with RSSI, noise and channel information |
| `iperf` | Fixed TCP/UDP client or server throughput test |
| `iperf_cmd` | Serial console for TCP/UDP client and server tests, optional peer port and memory logs |
| `NAPT_WiFi_to_HalowAP` | Wi-Fi STA uplink to HaLow AP NAPT |
| `NAPT_HalowSTA_to_WiFiAP` | HaLow STA uplink to Wi-Fi AP NAPT |
| `NAPT_WiFi_to_MeshGateway` | Wi-Fi STA uplink to mesh gateway NAPT |
| `NAPT_MeshPoint_to_WiFiAP` | Mesh point uplink to Wi-Fi AP NAPT |
| `UART_TCP_Client` / `UART_UDP_Client` | UART to TCP/UDP bridge |
| `UDP_Client_lowpower` | Low-power UDP client |
| `BasicHttpsClient` / `HalowBasicHttpClient` | HTTP/HTTPS client |
| `WebUpdate` / `httpUpdate` | OTA update |
| `As_VideoWebServer` / `halow_CameraWebServer` | Camera video web server |
| `halow-mqtt` | MQTT client |

## Performance

In current tests with 8 MHz bandwidth, the maximum TCP throughput is approximately **6.5 Mbps**. Actual throughput depends on region, channel, bandwidth, distance, antenna, interference and peer hardware.

## Instructions

The following table lists products of Heltec based on esp32s3:

|   MCU   |                       Relative boards                        |
| :-----: | :----------------------------------------------------------- |
| ESP32-S3 | [HT-HC32](https://heltec.org/project/ht-hc32/) |
| ESP32-S3 | [HT-HC33](https://heltec.org/project/ht-hc33/) |
| ESP32-S3 | [HT-RC3268](https://heltec.org/project/ht-rc3268/) |

## Installation Instructions

+ [Instructions for Windows](InstallGuide/windows.md)
+ [Instructions for Mac](InstallGuide/mac.md)
+ [Instructions for Debian/Ubuntu Linux](InstallGuide/debian_ubuntu.md)
+ [Instructions for Fedora](InstallGuide/fedora.md)
+ [Instructions for openSUSE](InstallGuide/opensuse.md)

## Decoding exceptions
You can refer to this article to learn about Arduino plugins and coding rules: [EspExceptionDecoder](https://github.com/me-no-dev/EspExceptionDecoder).

## Issue/Bug report template

Before reporting an issue, make sure you've searched for similar one that was already created. Also make sure to go through all the issues labelled as [for reference](https://github.com/HelTecAutomation/Heltec_nRF52/issues).

Also you can talk in our forum: [http://community.heltec.cn/](http://community.heltec.cn/)&nbsp;

## Contact us

- **Website：[https://heltec.org](https://heltec.org/)**
- **Document Page: [https://docs.heltec.cn](https://docs.heltec.org)**
- **Forum: [http://community.heltec.cn/](http://community.heltec.cn/)**
- **Twitter: [https://twitter.com/HeltecOrg](https://twitter.com/HeltecOrg)**
- **Face Book: [https://www.facebook.com/heltec.automation.5](https://www.facebook.com/heltec.automation.5)**

## 简体中文

由Heltec Automation制作的ESP32S3 Wifi Halow系列的Arduino源代码和工具链，包含一些基础代码、相关示例。

## 内容

  - [功能特性](#功能特性)
  - [示例概览](#示例概览)
  - [性能](#性能)
  - [说明](#说明)
  - [安装指南](#安装指南)
  - [编码规则](#编码规则)
  - [问题讨论 & BUG报告](#问题讨论-&-BUG报告)
  - [联系我们](#联系我们)

## 功能特性

- 支持 Wi-Fi HaLow STA、AP、802.11s Mesh Gateway 和 Mesh Point 模式。
- 支持区域、信道和带宽配置。区域/带宽/信道参考表位于 `examples/HalowAP/REGION_BW_CHANNEL_REF.h`。
- STA 和 Mesh Point 支持 DHCP 或静态 IP。
- AP 和 Mesh Gateway 支持内置 DHCP server。
- AP 模式最多支持 8 个 STA；Mesh 模式最多支持 8 个 peer。
- 支持 802.11s mesh peer 白名单、转发统计和逐 peer 链路统计。
- 支持 STA 和 mesh 链路的 RSSI、噪声、带宽和频率输出。
- 支持 Wi-Fi 与 HaLow 之间的 NAPT：
  - Wi-Fi STA 上行到 HaLow AP
  - HaLow STA 上行到 Wi-Fi AP
  - Wi-Fi STA 上行到 Mesh Gateway
  - Mesh Point 上行到 Wi-Fi AP
- 支持固定配置和串口命令两种 TCP/UDP 吞吐测试方式。
- 提供 Arduino 事件 API，支持 STA、AP 和 Mesh 状态变化，包括 mesh peer 连接/断开事件。
- 提供 HTTP/HTTPS client、Web server、Camera Web server、MQTT 和 HTTP/Web OTA 示例。
- 提供 UART 转 TCP/UDP 和低功耗 UDP client 示例。
- 支持 HaLow 扫描，并输出 RSSI、噪声和信道信息。

## 示例概览

| 示例 | 功能说明 |
| --- | --- |
| `HalowSTA` | 作为 STA 连接 HaLow AP |
| `HalowClient` / `HalowClientStaticIP` | DHCP 或静态 IP 的 TCP/UDP client |
| `HalowAP` | 创建 HaLow AP，可选 DHCP server |
| `HaLowMesh` | 802.11s Mesh Gateway/Point，支持 DHCP/静态 IP、peer 白名单和 peer 统计 |
| `HalowScan` | 扫描 HaLow AP，输出 RSSI、噪声和信道信息 |
| `iperf` | 固定配置的 TCP/UDP client 或 server 吞吐测试 |
| `iperf_cmd` | 串口命令控制 TCP/UDP client 和 server，可指定端口并输出内存信息 |
| `NAPT_WiFi_to_HalowAP` | Wi-Fi STA 上行到 HaLow AP 的 NAPT |
| `NAPT_HalowSTA_to_WiFiAP` | HaLow STA 上行到 Wi-Fi AP 的 NAPT |
| `NAPT_WiFi_to_MeshGateway` | Wi-Fi STA 上行到 Mesh Gateway 的 NAPT |
| `NAPT_MeshPoint_to_WiFiAP` | Mesh Point 上行到 Wi-Fi AP 的 NAPT |
| `UART_TCP_Client` / `UART_UDP_Client` | UART 转 TCP/UDP |
| `UDP_Client_lowpower` | 低功耗 UDP client |
| `BasicHttpsClient` / `HalowBasicHttpClient` | HTTP/HTTPS client |
| `WebUpdate` / `httpUpdate` | OTA 升级 |
| `As_VideoWebServer` / `halow_CameraWebServer` | Camera 视频 Web server |
| `halow-mqtt` | MQTT client |

## 性能

在当前 8 MHz 带宽测试中，最大 TCP 吞吐约为 **6.5 Mbps**。实际速率受区域、信道、带宽、距离、天线、干扰和对端硬件影响。

## 说明

下表列出了Heltec基于ESP32S3芯片的产品型号：

|   MCU   |                       Relative boards                        |
| :-----: | :----------------------------------------------------------- |
| ESP32-S3 | [HT-HC32](https://heltec.org/project/ht-hc32/) |
| ESP32-S3 | [HT-HC33](https://heltec.org/project/ht-hc33/) |
| ESP32-S3 | [HT-RC3268](https://heltec.org/project/ht-rc3268/) |

## 安装指南

- [Windows操作系统 -- 安装方法](InstallGuide/windows.md)
- [MacOS操作系统 -- 安装方法](InstallGuide/mac.md)
- [Linux操作系统(opensuse) -- 安装方法](InstallGuide/opensuse.md)
- [Linux操作系统(debian,ubuntu) -- 安装方法](InstallGuide/debian_ubuntu.md)
- [Linux操作系统(fedora) -- 安装方法](InstallGuide/fedora.md)

## 问题讨论 & BUG报告

在报告BUG之前，请先做详细的测试，如果问题真的存在，您可以通过以下方式报告或者讨论：

- [GitHub问题报告页](https://github.com/HelTecAutomation/ESP_HaLow/issues)
- [Heltec论坛 (仅英语交流)](http://community.heltec.cn/)

## 联系我们

- **官网：[https://heltec.org](https://heltec.org/)**
- **Heltec文档页: [https://docs.heltec.cn](https://docs.heltec.cn)**
- **Heltec论坛 (仅英语): [http://community.heltec.cn/](http://community.heltec.cn/)**
- **QQ群: 799093974(中文)**
