#ifndef JADE_ENV_H
#define JADE_ENV_H

#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

namespace EnvLoader {
    // Function to load the .env file into the environment variables
    bool loadEnvFile(const std::string& filename);

    // Function to get an environment variable's value by key
    std::string getEnvValue(const std::string& key);
}

#endif //JADE_ENV_H
