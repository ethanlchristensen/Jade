#include <iostream>
#include <curl/curl.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define OPENAIURL "https://api.openai.com/v1/chat/completions"
#define MODEL "gpt-3.5-turbo"

#define SYSTEMPROMPT "You are a young cool hip girl named Jade that is happy to talk with your friends on Discord."
#define SUMMARIZEPROMPT "You will be given a piece of text. Provide a brief summary of the text in 1 - 4 sentences."
#define EXTRACTPROMPT "You will be given a piece of text. Extract out 3 - 5 topics that represent the piece of text. Given them as a numbered list."

std::string llm(const std::string& apiToken, const std::string& system_prompt, const std::string& user_prompt);
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);