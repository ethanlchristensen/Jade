#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "utils/jade_util.h"

struct ChatMessage {
    std::string role;
    std::string content;
    std::vector<std::string> images;
};

class GeminiAPI {
private:
    std::string baseUrl;
    std::string apiKey;

    static std::string constructPayload(const std::string& model, const ChatMessage& message, bool stream = false);
    static std::string constructPayload(const std::string& model, const std::string& role, const std::string& content, bool stream = false);

public:
    explicit GeminiAPI(std::string baseURL, std::string apiKey);

    std::string sendMessage(const std::string& model, const std::string& role, const std::string& content, bool stream = false);
    std::string sendMessage(const std::string& model, const ChatMessage& message, bool stream = false);
    std::string checkImage(const std::string& model, const std::string& imageData);
};