#ifndef JADE_OLLAMA_H
#define JADE_OLLAMA_H

#include <utility>
#include <string>
#include <nlohmann/json.hpp>
#include "utils/jade_util.h"

class OllamaAPI {
public:
    explicit OllamaAPI(std::string baseURL);

    struct ChatMessage {
        std::string role;
        std::string content;
        std::vector<std::string> images;  // Base64 encoded images
    };

    std::string sendMessage(const std::string& model, const std::string& role,
                            const std::string& content, bool stream = true);

    std::string sendMessage(const std::string& model, const ChatMessage& message,
                            bool stream = true);

private:
    std::string baseUrl;
    APIClient apiClient;

    static std::string constructPayload(const std::string& model, const std::string& role,
                                 const std::string& content, bool stream);
    static std::string constructPayload(const std::string& model, const ChatMessage& message,
                                 bool stream);
};

#endif //JADE_OLLAMA_H
