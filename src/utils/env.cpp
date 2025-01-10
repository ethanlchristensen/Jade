#include "utils/env.h"

namespace EnvLoader {

    bool loadEnvFile(const std::string& filename) {
        std::ifstream envFile(filename);
        if (!envFile.is_open()) {
            return false;
        }

        std::string line;
        while (std::getline(envFile, line)) {
            // Ignore empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }

            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);

                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

#if defined(_WIN32) || defined(_WIN64)
                _putenv_s(key.c_str(), value.c_str());
#else
                setenv(key.c_str(), value.c_str(), 1);
#endif
            }
        }

        envFile.close();
        return true;
    }

    std::string getEnvValue(const std::string& key) {
        const char* value = std::getenv(key.c_str());
        if (value == nullptr) {
            return "";
        }
        return value;
    }
}
