#include "descriptor.hpp"
#include "exceptions.hpp"

#include <unistd.h>
#include <utility>

namespace tcp {
    Descriptor::Descriptor(int fd): _fd(fd) {}
    Descriptor::Descriptor(Descriptor &&other): _fd(other._fd) {
        other._fd = -1;
    }
    Descriptor::~Descriptor () {
        if (!broken())
            close();
    }

    // Setter and getter
    void Descriptor::set_fd(int fd) {
        if (!broken())
            close();
        _fd = fd;
    }
    int Descriptor::get_fd() const {
        if (broken())
            throw BadDescriptorUsed();
        
        return _fd;
    }

    Descriptor& Descriptor::operator= (Descriptor &&other) {
        if (!broken())
            close();
        _fd = std::exchange(other._fd, -1);

        return *this;
    }

    bool Descriptor::broken() const noexcept {
        return _fd < 0;
    }

    void Descriptor::close() {
        if (broken())
            throw BadDescriptorUsed();

        ::close(_fd);
        _fd = -1;
    }
};