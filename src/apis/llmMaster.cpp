#include "apis.h"
#include "helpers.cpp"

std::string llm(const std::string& apiToken, const std::string& system_prompt, const std::string& user_prompt) {
    CURL *curl;
    CURLcode response;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = nullptr;
        std::string auth = fmt::format("Authorization: Bearer {}", apiToken);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth.c_str());
        std::string data = fmt::format(
            R"({{"model": "{}","messages": [{{"role": "system","content": "{}" }},{{"role": "user", "content": "{}" }}],"temperature": 1,"max_tokens": 500, "top_p": 1,"frequency_penalty": 0,"presence_penalty": 0 }})",
            MODEL, system_prompt, user_prompt
        );

        curl_easy_setopt(curl, CURLOPT_URL, OPENAIURL);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        response = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        // did we get response
        if(response == 0) {
            json responseJson = json::parse(readBuffer);
            return responseJson["choices"][0]["message"]["content"];
        } else {
            return "Hey, I am not in the mood to talk right now.";
        }
    }
    return "Hey, I am not in the mood to talk right now.";
}
