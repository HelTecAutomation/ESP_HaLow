/*
  Server.h - Server class for Raspberry Pi
  Copyright (c) 2016 Hristo Gochkov  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef _HalowSERVER_H_
#define _HalowSERVER_H_

#include "Arduino.h"
#include "Server.h"
#include "HalowClient.h"
#include "IPAddress.h"

class HalowServer {
  private:
    int sockfd;
    int _accepted_sockfd = -1;
    IPAddress _addr;
    uint16_t _port;
    uint8_t _max_clients;
    bool _listening;
    bool _noDelay = false;

  public:
    void listenOnLocalhost(){}

    HalowServer(uint16_t port=80, uint8_t max_clients=4):sockfd(-1),_accepted_sockfd(-1),_addr(),_port(port),_max_clients(max_clients),_listening(false),_noDelay(false) {
      log_v("HalowServer::HalowServer(port=%d, ...)", port);
    }
    HalowServer(const IPAddress& addr, uint16_t port=80, uint8_t max_clients=4):sockfd(-1),_accepted_sockfd(-1),_addr(addr),_port(port),_max_clients(max_clients),_listening(false),_noDelay(false) {
      log_v("HalowServer::HalowServer(addr=%s, port=%d, ...)", addr.toString().c_str(), port);
    }
    ~HalowServer(){ end();}
    HalowClient available() __attribute__((deprecated("Renamed to accept().")));
    HalowClient accept();
    void begin(uint16_t port=0);
    void begin(uint16_t port, int reuse_enable);
    void setNoDelay(bool nodelay);
    bool getNoDelay();
    bool hasClient();

    void end();
    void close();
    void stop();
    operator bool(){return _listening;}
    int setTimeout(uint32_t seconds);
};

#endif /* _HalowSERVER_H_ */
