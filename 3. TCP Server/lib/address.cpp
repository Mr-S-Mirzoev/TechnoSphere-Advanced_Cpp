#include "address.hpp"
#include <arpa/inet.h>

namespace tcp {

    Address::Address(const std::string &s_addr, int port) noexcept:
        _str_addr(s_addr), _port(port), _addr(new sockaddr_in) {
        _addr->sin_family = AF_INET;
        _addr->sin_port = htons(_port);
        ::inet_aton(s_addr.c_str(), &_addr->sin_addr);
    }

    Address::Address(Address &other) noexcept: 
        _addr(new struct sockaddr_in{other._addr}), _str_addr(other._str_addr), 
        _port(other._port) {}
    
    struct sockaddr_in *Address::get_struct() noexcept {
        return _addr.get();
    }

    Address::~Address() = default;
}