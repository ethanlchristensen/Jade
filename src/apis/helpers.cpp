#include <iostream>
#include <string>
#include <curl/curl.h>

class APIClient {
public:
    APIClient() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~APIClient() {
        curl_global_cleanup();
    }

    std::string GET(const std::string &url, const std::string &authToken = "") {
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

    std::string POST(const std::string &url, const std::string &data, const std::string &authToken = "") {
        std::cout << "POST called!";
        CURL *curl = curl_easy_init();
        if (!curl) {
            return "Error initializing cURL";
        }

        std::cout << "[POST] " + url + "\n";

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

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
        size_t totalSize = size * nmemb;
        out->append((char*)contents, totalSize);
        return totalSize;
    }
};