/*
  Client.h - Client class for Raspberry Pi
  Copyright (c) 2016 Hristo Gochkov  All right reserved.
  Copyright (c) Heltec AutoMation.  All right reserved.
  
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

#include "HalowClient.h"
#include "Halow.h"
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <errno.h>

#define IN6_IS_ADDR_V4MAPPED(a) \
        ((((__const uint32_t *) (a))[0] == 0) \
         && (((__const uint32_t *) (a))[1] == 0) \
         && (((__const uint32_t *) (a))[2] == htonl (0xffff)))

#define Halow_CLIENT_DEF_CONN_TIMEOUT_MS  (3000)
#define Halow_CLIENT_MAX_WRITE_RETRY      (10)
#define Halow_CLIENT_SELECT_TIMEOUT_US    (1000000)
#define Halow_CLIENT_FLUSH_BUFFER_SIZE    (1024)

#undef connect
#undef write
#undef read

class HalowClientRxBuffer {
private:
        size_t _size;
        uint8_t *_buffer;
        size_t _pos;
        size_t _fill;
        int _fd;
        bool _failed;

        size_t r_available()
        {
            if(_fd < 0){
                return 0;
            }
            int count;
#ifdef ESP_IDF_VERSION_MAJOR
            int res = lwip_ioctl(_fd, FIONREAD, &count);
#else
            int res = lwip_ioctl_r(_fd, FIONREAD, &count);
#endif
            if(res < 0) {
                _failed = true;
                return 0;
            }
            return count;
        }

        size_t fillBuffer()
        {
            if(!_buffer){
                _buffer = (uint8_t *)malloc(_size);
                if(!_buffer) {
                    log_e("Not enough memory to allocate buffer");
                    _failed = true;
                    return 0;
                }
            }
            if(_fill && _pos == _fill){
                _fill = 0;
                _pos = 0;
            }
            if(!_buffer || _size <= _fill || !r_available()) {
                return 0;
            }
            int res = recv(_fd, _buffer + _fill, _size - _fill, MSG_DONTWAIT);
            if(res < 0) {
                if(errno != EWOULDBLOCK) {
                    _failed = true;
                }
                return 0;
            }
            _fill += res;
            return res;
        }

public:
    HalowClientRxBuffer(int fd, size_t size=1436)
        :_size(size)
        ,_buffer(NULL)
        ,_pos(0)
        ,_fill(0)
        ,_fd(fd)
        ,_failed(false)
    {
        //_buffer = (uint8_t *)malloc(_size);
    }

    ~HalowClientRxBuffer()
    {
        free(_buffer);
    }

    bool failed(){
        return _failed;
    }

    int read(uint8_t * dst, size_t len){
        if(!dst || !len || (_pos == _fill && !fillBuffer())){
            return _failed ? -1 : 0;
        }
        size_t a = _fill - _pos;
        if(len <= a || ((len - a) <= (_size - _fill) && fillBuffer() >= (len - a))){
            if(len == 1){
                *dst = _buffer[_pos];
            } else {
                memcpy(dst, _buffer + _pos, len);
            }
            _pos += len;
            return len;
        }
        size_t left = len;
        size_t toRead = a;
        uint8_t * buf = dst;
        memcpy(buf, _buffer + _pos, toRead);
        _pos += toRead;
        left -= toRead;
        buf += toRead;
        while(left){
            if(!fillBuffer()){
                return len - left;
            }
            a = _fill - _pos;
            toRead = (a > left)?left:a;
            memcpy(buf, _buffer + _pos, toRead);
            _pos += toRead;
            left -= toRead;
            buf += toRead;
        }
        return len;
    }

    int peek(){
        if(_pos == _fill && !fillBuffer()){
            return -1;
        }
        return _buffer[_pos];
    }

    size_t available(){
        return _fill - _pos + r_available();
    }

    void flush(){
        if(r_available()){
            fillBuffer();
        }
        _pos = _fill;
    }
};

class HalowClientSocketHandle {
private:
    int sockfd;

public:
    HalowClientSocketHandle(int fd):sockfd(fd)
    {
    }

    ~HalowClientSocketHandle()
    {
        close(sockfd);
    }

    int fd()
    {
        return sockfd;
    }
};

HalowClient::HalowClient():_rxBuffer(nullptr),_connected(false),_sse(false),_timeout(Halow_CLIENT_DEF_CONN_TIMEOUT_MS),next(NULL)
{
}

HalowClient::HalowClient(int fd):_connected(true),_timeout(Halow_CLIENT_DEF_CONN_TIMEOUT_MS),next(NULL)
{
    clientSocketHandle.reset(new HalowClientSocketHandle(fd));
    _rxBuffer.reset(new HalowClientRxBuffer(fd));
}

HalowClient::~HalowClient()
{
    stop();
}

void HalowClient::stop()
{
    clientSocketHandle = NULL;
    _rxBuffer = NULL;
    _connected = false;
    _lastReadTimeout = 0;
    _lastWriteTimeout = 0;
}

int HalowClient::connect(IPAddress ip, uint16_t port)
{
    return connect(ip,port,_timeout);
}

int HalowClient::connect(IPAddress ip, uint16_t port, int32_t timeout_ms)
{
    struct sockaddr_storage serveraddr = {};
    _timeout = timeout_ms;
    int sockfd = -1;

    if (ip.type() == IPv6) {
        struct sockaddr_in6 *tmpaddr = (struct sockaddr_in6 *)&serveraddr;
        sockfd = socket(AF_INET6, SOCK_STREAM, 0);
        tmpaddr->sin6_family = AF_INET6;
        memcpy(tmpaddr->sin6_addr.un.u8_addr, &ip[0], 16);
        tmpaddr->sin6_port = htons(port);
        tmpaddr->sin6_scope_id = ip.zone();
    } else {
        struct sockaddr_in *tmpaddr = (struct sockaddr_in *)&serveraddr;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        tmpaddr->sin_family = AF_INET;
        tmpaddr->sin_addr.s_addr = ip;
        tmpaddr->sin_port = htons(port);
    }
    if (sockfd < 0) {
        log_e("socket: %d", errno);
        return 0;
    }
    fcntl( sockfd, F_SETFL, fcntl( sockfd, F_GETFL, 0 ) | O_NONBLOCK );

    fd_set fdset;
    struct timeval tv;
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);
    tv.tv_sec = _timeout / 1000;
    tv.tv_usec = (_timeout  % 1000) * 1000;

#ifdef ESP_IDF_VERSION_MAJOR
    int res = lwip_connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
#else
    int res = lwip_connect_r(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
#endif
    if (res < 0 && errno != EINPROGRESS) {
        log_e("connect on fd %d, errno: %d, \"%s\"", sockfd, errno, strerror(errno));
        close(sockfd);
        return 0;
    }

    res = select(sockfd + 1, nullptr, &fdset, nullptr, _timeout<0 ? nullptr : &tv);
    if (res < 0) {
        log_e("select on fd %d, errno: %d, \"%s\"", sockfd, errno, strerror(errno));
        close(sockfd);
        return 0;
    } else if (res == 0) {
        log_i("select returned due to timeout %d ms for fd %d", _timeout, sockfd);
        close(sockfd);
        return 0;
    } else {
        int sockerr;
        socklen_t len = (socklen_t)sizeof(int);
        res = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &sockerr, &len);

        if (res < 0) {
            log_e("getsockopt on fd %d, errno: %d, \"%s\"", sockfd, errno, strerror(errno));
            close(sockfd);
            return 0;
        }

        if (sockerr != 0) {
            log_e("socket error on fd %d, errno: %d, \"%s\"", sockfd, sockerr, strerror(sockerr));
            close(sockfd);
            return 0;
        }
    }

#define ROE_HalowCLIENT(x,msg) { if (((x)<0)) { log_e("Setsockopt '" msg "'' on fd %d failed. errno: %d, \"%s\"", sockfd, errno, strerror(errno)); return 0; }}
    ROE_HalowCLIENT(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)),"SO_SNDTIMEO");
    ROE_HalowCLIENT(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)),"SO_RCVTIMEO");

    // These are also set in HalowClientSecure, should be set here too?
    //ROE_HalowCLIENT(setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)),"TCP_NODELAY"); 
    //ROE_HalowCLIENT (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable)),"SO_KEEPALIVE");

    fcntl( sockfd, F_SETFL, fcntl( sockfd, F_GETFL, 0 ) & (~O_NONBLOCK) );
    clientSocketHandle.reset(new HalowClientSocketHandle(sockfd));
    _rxBuffer.reset(new HalowClientRxBuffer(sockfd));

    _connected = true;
    return 1;
}

int HalowClient::connect(const char *host, uint16_t port)
{
    return connect(host,port,_timeout);
}

int HalowClient::connect(const char *host, uint16_t port, int32_t timeout_ms)
{
    IPAddress srv((uint32_t)0);
    if(!HalowGenericClass::hostByName(host, srv)){
        return 0;
    }
    return connect(srv, port, timeout_ms);
}

int HalowClient::setSocketOption(int option, char* value, size_t len)
{
    return setSocketOption(SOL_SOCKET, option, (const void*)value, len);
}

int HalowClient::setSocketOption(int level, int option, const void* value, size_t len)
{
    int res = setsockopt(fd(), level, option, value, len);
    if(res < 0) {
        log_e("fail on %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
    }
    return res;
}

int HalowClient::getSocketOption(int level, int option, const void* value, size_t size)
{
    int res = getsockopt(fd(), level, option, (char *)value, (socklen_t*)&size);
    if(res < 0) {
        log_e("fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
    }
    return res;
}

int HalowClient::setOption(int option, int *value)
{
    return setSocketOption(IPPROTO_TCP, option, (const void*)value, sizeof(int));
}

int HalowClient::getOption(int option, int *value)
{
    socklen_t size = sizeof(int);
    int res = getsockopt(fd(), IPPROTO_TCP, option, (char *)value, &size);
    if(res < 0) {
        log_e("fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
    }
    return res;
}

void HalowClient::setConnectionTimeout(uint32_t milliseconds)
{
    _timeout = milliseconds;
}

int HalowClient::setNoDelay(bool nodelay)
{
    int flag = nodelay;
    return setOption(TCP_NODELAY, &flag);
}

bool HalowClient::getNoDelay()
{
    int flag = 0;
    getOption(TCP_NODELAY, &flag);
    return flag;
}

size_t HalowClient::write(uint8_t data)
{
    return write(&data, 1);
}

int HalowClient::read()
{
    uint8_t data = 0;
    int res = read(&data, 1);
    if(res < 0) {
        return res;
    }
    if (res == 0) {  //  No data available.
        return -1;
    }
    return data;
}

size_t HalowClient::write(const uint8_t *buf, size_t size)
{
    int res =0;
    int retry = Halow_CLIENT_MAX_WRITE_RETRY;
    int socketFileDescriptor = fd();
    size_t totalBytesSent = 0;
    size_t bytesRemaining = size;

    if(!_connected || (socketFileDescriptor < 0)) {
        return 0;
    }

    while(retry) {
        //use select to make sure the socket is ready for writing
        fd_set set;
        struct timeval tv;
        FD_ZERO(&set);        // empties the set
        FD_SET(socketFileDescriptor, &set); // adds FD to the set
        tv.tv_sec = 0;
        tv.tv_usec = Halow_CLIENT_SELECT_TIMEOUT_US;
        retry--;

        if(_lastWriteTimeout != _timeout){
            if(fd() >= 0){
                struct timeval timeout_tv;
                timeout_tv.tv_sec = _timeout / 1000;
                timeout_tv.tv_usec = (_timeout % 1000) * 1000;
                if(setSocketOption(SO_SNDTIMEO, (char *)&timeout_tv, sizeof(struct timeval)) >= 0)
                {
                    _lastWriteTimeout = _timeout;
                }
            }
        }

        if(select(socketFileDescriptor + 1, NULL, &set, NULL, &tv) < 0) {
            return 0;
        }

        if(FD_ISSET(socketFileDescriptor, &set)) {
            res = send(socketFileDescriptor, (void*) buf, bytesRemaining, MSG_DONTWAIT);
            if(res > 0) {
                totalBytesSent += res;
                if (totalBytesSent >= size) {
                    //completed successfully
                    retry = 0;
                } else {
                    buf += res;
                    bytesRemaining -= res;
                    retry = Halow_CLIENT_MAX_WRITE_RETRY;
                }
            }
            else if(res < 0) {
                log_e("fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
                if(errno != EAGAIN) {
                    //if resource was busy, can try again, otherwise give up
                    stop();
                    res = 0;
                    retry = 0;
                }
            }
            else {
                // Try again
            }
        }
    }
    return totalBytesSent;
}

size_t HalowClient::write_P(PGM_P buf, size_t size)
{
    return write(buf, size);
}

size_t HalowClient::write(Stream &stream)
{
    uint8_t * buf = (uint8_t *)malloc(1360);
    if(!buf){
        return 0;
    }
    size_t toRead = 0, toWrite = 0, written = 0;
    size_t available = stream.available();
    while(available){
        toRead = (available > 1360)?1360:available;
        toWrite = stream.readBytes(buf, toRead);
        written += write(buf, toWrite);
        available = stream.available();
    }
    free(buf);
    return written;
}

int HalowClient::read(uint8_t *buf, size_t size)
{
    if(_lastReadTimeout != _timeout){
        if(fd() >= 0){
            struct timeval timeout_tv;
            timeout_tv.tv_sec = _timeout / 1000;
            timeout_tv.tv_usec = (_timeout % 1000) * 1000;
            if(setSocketOption(SO_RCVTIMEO, (char *)&timeout_tv, sizeof(struct timeval)) >= 0)
            {
                _lastReadTimeout = _timeout;
            }
        }
    }

    int res = -1;
    if (_rxBuffer) {
        res = _rxBuffer->read(buf, size);
        if(_rxBuffer->failed()) {
            log_e("fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
            stop();
        }
    }
    return res;
}

int HalowClient::peek()
{
    int res = -1;
    if (_rxBuffer) {
        res = _rxBuffer->peek();
        if(_rxBuffer->failed()) {
            log_e("fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
            stop();
        }
    }
    return res;
}

int HalowClient::available()
{
    if(!_rxBuffer)
    {
        return 0;
    }
    int res = _rxBuffer->available();
    if(_rxBuffer->failed()) {
        log_e("fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
        stop();
    }
    return res;
}

// Though flushing means to send all pending data,
// seems that in Arduino it also means to clear RX
void HalowClient::flush() {
    if (_rxBuffer != nullptr) {
        _rxBuffer->flush();
    }
}

uint8_t HalowClient::connected()
{
    if (_connected) {
        uint8_t dummy;
        int res = recv(fd(), &dummy, 0, MSG_DONTWAIT);
        // avoid unused var warning by gcc
        (void)res;
        
        //Serial.printf("res %d %d;",res,errno);
        // recv only sets errno if res is <= 0
        if (res <= 0){
          switch (errno) {
              case EWOULDBLOCK:
              case ENOENT: //caused by vfs
                  _connected = true;
                  break;
              case ENOTCONN:
              case EPIPE:
              case ECONNRESET:
              case ECONNREFUSED:
              case ECONNABORTED:
                  _connected = false;
                  log_d("Disconnected: RES: %d, ERR: %d", res, errno);
                  break;
              default:
                  log_i("Unexpected: RES: %d, ERR: %d", res, errno);
                  _connected = true;
                  break;
          }
        } else {
          _connected = true;
        }
    }
    return _connected;
}

IPAddress HalowClient::remoteIP(int fd) const
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    getpeername(fd, (struct sockaddr*)&addr, &len);

    // IPv4 socket, old way
    if (((struct sockaddr*)&addr)->sa_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        return IPAddress((uint32_t)(s->sin_addr.s_addr));
    }

    // IPv6, but it might be IPv4 mapped address
    if (((struct sockaddr*)&addr)->sa_family == AF_INET6) {
        struct sockaddr_in6 *saddr6 = (struct sockaddr_in6 *)&addr;
        if (IN6_IS_ADDR_V4MAPPED(saddr6->sin6_addr.un.u32_addr)) {
            return IPAddress(IPv4, (uint8_t*)saddr6->sin6_addr.s6_addr+IPADDRESS_V4_BYTES_INDEX);
        } else {
            return IPAddress(IPv6, (uint8_t*)(saddr6->sin6_addr.s6_addr), saddr6->sin6_scope_id);
        }
    }
    log_e("HalowClient::remoteIP Not AF_INET or AF_INET6?");
    return (IPAddress(0,0,0,0));
}

uint16_t HalowClient::remotePort(int fd) const
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    getpeername(fd, (struct sockaddr*)&addr, &len);
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    return ntohs(s->sin_port);
}

IPAddress HalowClient::remoteIP() const
{
    return remoteIP(fd());
}

uint16_t HalowClient::remotePort() const
{
    return remotePort(fd());
}

IPAddress HalowClient::localIP(int fd) const
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    getsockname(fd, (struct sockaddr*)&addr, &len);
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    return IPAddress((uint32_t)(s->sin_addr.s_addr));
}

uint16_t HalowClient::localPort(int fd) const
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    getsockname(fd, (struct sockaddr*)&addr, &len);
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    return ntohs(s->sin_port);
}

IPAddress HalowClient::localIP() const
{
    return localIP(fd());
}

uint16_t HalowClient::localPort() const
{
    return localPort(fd());
}

bool HalowClient::operator==(const HalowClient& rhs)
{
    return clientSocketHandle == rhs.clientSocketHandle && remotePort() == rhs.remotePort() && remoteIP() == rhs.remoteIP();
}

int HalowClient::fd() const
{
    if (clientSocketHandle == NULL) {
        return -1;
    } else {
        return clientSocketHandle->fd();
    }
}

void HalowClient::setSSE(bool sse)
{
    _sse = sse;
}

bool HalowClient::isSSE()
{
    return _sse;
}

void __attribute__((weak)) halow_esp_sleep()
{

}