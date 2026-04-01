#include <Interface.hpp>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

Sender::Sender(std::string ip_address, uint16_t port) {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip_address.c_str());
    address.sin_port = htons(port);

    size = sizeof(address);

    _socket = socket(AF_INET, SOCK_DGRAM, 0);
}

Sender::~Sender() { close(_socket); }

void Sender::sendVector(float x, float y, float z)
{   
    float vector[3] = {x, y, z};
    sendto(_socket, &vector, sizeof(vector), MSG_DONTWAIT, (const sockaddr*) &address, size);
}