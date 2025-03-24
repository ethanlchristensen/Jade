#ifndef JADE_JADE_UTIL_H
#define JADE_JADE_UTIL_H

#include <array>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <curl/curl.h>
#include <fmt/format.h>
#include <fstream>
#ifdef _WIN32
#include <cppcodec/base64_rfc4648.hpp>
using base64 = cppcodec::base64_rfc4648;
#else
extern "C" {
#include <b64/cencode.h>
}
#include <sstream>
#endif

extern std::vector<std::string> devMessages;
extern std::vector<std::string> prodMessages;

std::string executeCommand(const std::string &command);

std::string secondsToHHMMSS(int total_seconds);

std::string encode_to_base64(const std::string &data);

std::string extractFirstFrameFromVideo(const std::string& videoUrl);

class APIClient {
public:
    APIClient();

    ~APIClient();

    static std::string GET(const std::string &url, const std::string &authToken = "");

    static std::string POST(const std::string &url, const std::string &data, const std::string &authToken = "", const std::vector<std::string> &additionalHeaders = {});

    static std::string download_image(const std::string &url);

private:
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *out);

    static size_t WriteCallbackToVector(void *contents, size_t size, size_t nmemb, std::vector<char> *out);
};


#endif //JADE_JADE_UTIL_H