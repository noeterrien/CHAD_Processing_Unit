#ifndef DEF_PARAMETERS
#define DEF_PARAMETERS

#include <string>
#include <cstdint>

struct Parameters {

    // constructor
    Parameters(std::string path);

    // parameters
    std::string source;
    std::string rov_ip;
    uint8_t rov_send_port;
    uint8_t cockpit_reset_reference_port;
    
    float gainX;
    float gainY;
    float gainZ;

};


#endif