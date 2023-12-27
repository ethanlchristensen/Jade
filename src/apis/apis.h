#include <iostream>
#include <curl/curl.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define OPENAIURL "https://api.openai.com/v1/chat/completions"
#define MODEL "gpt-3.5-turbo"

std::string chat(const std::string& apiToken, const std::string& user_prompt);
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);