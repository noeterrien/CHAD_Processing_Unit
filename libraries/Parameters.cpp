#include <Parameters.hpp>
#include <yaml-cpp/yaml.h>
#include <string>
#include <iostream>

Parameters::Parameters(std::string const path) {
    params_map = YAML::LoadFile("config.yaml");
}

// int main() {
//     Parameters p("config.yaml");
//     std::cout << "camera_source : " << p.get<std::string>("camera_source") << std::endl;
//     std::cout << "keep_match_threshold : " << p.get<float>("keep_match_threshold") << std::endl;
//     p.set("keep_match_threshold", 0.6);
//     std::cout << "new keep_match_threshold : " << p.get<float>("keep_match_threshold") << std::endl;
//     p.get<std::string>("coucou");
//     return 0;
// }