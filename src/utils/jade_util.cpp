#include "utils/jade_util.h"

std::vector<std::string> devMessages = {
        "Engines warming up... Dev in progress.",
        "Stars aligning. Dev mode active!",
        "Jade’s coding an update!",
        "Building Jade’s next mission!",
        "Navigating dev space. Stay tuned!",
        "Jade’s in dev mode. Prepare for lift-off!",
        "Systems upgrading. Dev phase on!",
        "Cosmic code in the works!",
        "Jade’s core systems in dev!",
        "Dev mode engaged. Updates soon!",
        "Crafting the future in dev!",
        "Cosmic engine in beta. Hold tight!",
        "Exploring deep code. Stay tuned!",
        "Jade’s in the lab, refining things.",
        "Countdown to release. Dev in progress!",
        "Coding at light speed!",
        "Mission control: Jade in dev.",
        "Jade’s future in the lab!",
        "Cosmic experiments in progress!",
        "Jade’s in dev mode. Stay tuned!"
};

std::vector<std::string> prodMessages = {
        "Jade is ready for launch!",
        "Systems online. Jade is live!",
        "Mission complete. Jade operational!",
        "Engines powered. Jade at service!",
        "Jade is live and ready!",
        "All systems go. Jade’s live!",
        "Jade’s mission active!",
        "Ready for the stars. Jade’s in orbit!",
        "Jade is operational!",
        "Jade is in full swing!",
        "Jade has entered the cosmos!",
        "Mission success! Jade on standby.",
        "Jade launched. Let’s go!",
        "All systems functioning. Jade live!",
        "Jade fully operational. Let’s go!",
        "Jade at command. Ready for mission!",
        "Jade’s online, exploring stars!",
        "Lift-off complete. Jade’s live!",
        "Code’s solid. Jade’s ready!",
        "Jade operational and engaged!"
};

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

std::string APIClient::download_image(const std::string &url) {
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
    if (seconds > 0 || result.empty()) {
        if (!result.empty()) result += ", ";
        result += fmt::format("{} Second{}", seconds, seconds > 1 ? "s" : "");
    }

    return result;
}