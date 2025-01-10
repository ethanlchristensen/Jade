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
    bool loadEnvFile(const std::string& filename);
    std::string getEnvValue(const std::string& key);
}

#endif //JADE_ENV_H
