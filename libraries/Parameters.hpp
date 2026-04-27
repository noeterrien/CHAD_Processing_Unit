#ifndef DEF_PARAMETERS
#define DEF_PARAMETERS

#include <yaml-cpp/yaml.h>
#include <string>
#include <iostream>

class Parameters {

public:
    Parameters(std::string const path);

    template <typename T=std::string>
    T get(std::string const parameter_name) const {
        if (params_map[parameter_name]) {
            return params_map[parameter_name].as<T>();
        } else {
            T default_value; 
            std::cerr << "Unknown parameter : " << parameter_name << ", returning default value : " << default_value << std::endl;
            return default_value;
        }
    }

    template <typename T>
    bool set(std::string const parameter_name, T value) {
        if (params_map[parameter_name]) {
            params_map[parameter_name] = value;
            return true;
        } else {
            std::cerr << "Unknown paramameter : " << parameter_name << ", could not assign value" << std::endl;
            return false; 
        }
    }

private:
    YAML::Node params_map;
};


#endif