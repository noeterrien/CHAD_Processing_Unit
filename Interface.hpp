#ifndef DEF_SOCKET
#define DEF_SOCKET

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

/*The instances defined in this class use IPv4 (AF_INET) and UDP protocol (SOCK_DGRAM)*/

/*An instance to send messages to an address*/
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