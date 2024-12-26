#include "utils/jade_util.h"

std::string executeCommand(const std::string& command) {
    std::array<char, 128> buffer{};
    std::string result;
#if defined(_WIN32) || defined(_WIN64)
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (!pipe) {
        throw std::runtime_error("failed to open the pipe!");
    }
    try {
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            result.append(buffer.data());
            buffer.fill(0);  // Clear the buffer after each use
        }
    } catch (...) {
#if defined(_WIN32) || defined(_WIN64)
        _pclose(pipe);
#else
        pclose(pipe);
#endif
        throw;
    }
#if defined(_WIN32) || defined(_WIN64)
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    std::string finalResult = std::move(result);  // Ensure complete move of data
    return finalResult;
}

std::string encode_to_base64(const std::string& data) {
    std::string encoded = cppcodec::base64_rfc4648::encode(data);
    return encoded;
}

APIClient::APIClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

APIClient::~APIClient() {
    curl_global_cleanup();
}

std::string APIClient::GET(const std::string &url, const std::string &authToken) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return "Error initializing cURL";
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    struct curl_slist *headers = nullptr;
    if (!authToken.empty()) {
        std::string bearerToken = "Authorization: Bearer " + authToken;
        headers = curl_slist_append(headers, bearerToken.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        response = curl_easy_strerror(res);
    }

    curl_easy_cleanup(curl);
    if (headers) {
        curl_slist_free_all(headers);
    }
    return response;
}

std::string APIClient::POST(const std::string &url, const std::string &data, const std::string &authToken) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return "Error initializing cURL";
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    struct curl_slist *headers = nullptr;
    if (!authToken.empty()) {
        std::string bearerToken = "Authorization: Bearer " + authToken;
        headers = curl_slist_append(headers, bearerToken.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        response = curl_easy_strerror(res);
    }

    curl_easy_cleanup(curl);
    if (headers) {
        curl_slist_free_all(headers);
    }
    return response;
}

std::string APIClient::download_image(std::string &url) {
    CURL *curl = curl_easy_init();
    std::string imageData;

    if (!curl) {
        return imageData;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "cURL error: " << curl_easy_strerror(res) << "\n";
    }

    curl_easy_cleanup(curl);
    return imageData;
}

size_t APIClient::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    size_t totalSize = size * nmemb;
    out->append((char*)contents, totalSize);
    return totalSize;
}

size_t APIClient::WriteCallbackToVector(void* contents, size_t size, size_t nmemb, std::vector<char>* out) {
    size_t totalSize = size * nmemb;
    out->insert(out->end(), (char*)contents, (char*)contents + totalSize);
    return totalSize;
}

std::string secondsToHHMMSS(int total_seconds) {
    int hours = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;

    std::string result;

    if (hours > 0) {
        result += fmt::format("{} Hour{}", hours, hours > 1 ? "s" : "");
    }
    if (minutes > 0) {
        if (!result.empty()) result += ", ";
        result += fmt::format("{} Minute{}", minutes, minutes > 1 ? "s" : "");
    }
    if (seconds > 0 || result.empty()) { // Include seconds if they're non-zero or if all other fields are zero
        if (!result.empty()) result += ", ";
        result += fmt::format("{} Second{}", seconds, seconds > 1 ? "s" : "");
    }

    return result;
}