#ifndef DEF_INTERFACE
#define DEF_INTERFACE

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <httplib.h>

/*An instance to send messages to an address using use IPv4 (AF_INET) and UDP protocol (SOCK_DGRAM)*/
class Sender {

public:
    Sender(std::string ip_address, uint16_t port);
    ~Sender();

    void sendVector(float x, float y, float z);

private:
    sockaddr_in address;
    socklen_t size;
    int _socket;
};

#endif