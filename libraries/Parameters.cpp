#include <Parameters.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

Parameters::Parameters(string path) {
    
    map<string, string> params;

    // reading config file
    string line;
    ifstream config_file(path.c_str());
    if (config_file.is_open())
    {
        while ( getline(config_file, line) )
        {
            string key_value[2];
            bool reading_value=false;
            for ( int i = 0; i < line.size(); i++) 
            {   
                if (line[i] == '#') break; // ignore the rest of the line
                if (line[i] == '=') reading_value=true;
                else {
                    key_value[reading_value].push_back(line[i]); 
                }
            }
            if (key_value[0] != "") params[key_value[0]] = key_value[1];
        }
        config_file.close();
    } else cerr << "Unable to open config file " << path << endl; 

    // check that every mandatory parameter was given
    vector<string> mandatory{"source", "rov_ip", "rov_send_port", "cockpit_reset_reference_port", "gainX", "gainY", "gainZ"};
    for (int it=0; it < mandatory.size(); it++)
    {
        if (params.count(mandatory[it])==0) cerr << "warning : no value provided for " << mandatory[it] << endl;
    }

    // parse config file
    for (auto it=params.begin(); it != params.end(); it++)
    {
        if (it -> first == "source") source = it -> second;
            
        else if (it -> first == "rov_ip") rov_ip = it -> second;
            
        else if (it -> first == "rov_send_port") rov_send_port = atoi((it -> second).c_str());    
            
        else if (it -> first == "cockpit_reset_reference_port") cockpit_reset_reference_port = atoi((it -> second).c_str());
            
        else if (it -> first == "gainX") gainX = atof((it -> second).c_str());
            
        else if (it -> first == "gainY") gainY = atof((it -> second).c_str());
            
        else if (it -> first == "gainZ") gainZ = atof((it -> second).c_str());
            
        else cerr << "warning : unrecognized parameter " << it -> first << " when reading " << path << endl;        
    }
}