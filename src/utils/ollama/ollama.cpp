#include "utils/ollama/ollama.h"

OllamaAPI::OllamaAPI(std::string  baseURL) : baseUrl(std::move(baseURL)) {
}

std::string
OllamaAPI::sendMessage(const std::string& model, const std::string& role,
                       const std::string& content, bool stream) {
    ChatMessage message{role, content, {}};
    return sendMessage(model, message, stream);
}

std::string
OllamaAPI::sendMessage(const std::string& model, const ChatMessage& message,
                       bool stream) {
    std::string endpoint = baseUrl + "/api/chat";
    std::string data = constructPayload(model, message, stream);
    return APIClient::POST(endpoint, data);
}

std::string
OllamaAPI::constructPayload(const std::string& model, const std::string& role,
                            const std::string& content, bool stream) {
    ChatMessage message{role, content, {}};
    return constructPayload(model, message, stream);
}

std::string
OllamaAPI::constructPayload(const std::string& model, const ChatMessage& message,
                            bool stream) {
    nlohmann::json payload;
    payload["model"] = model;

    nlohmann::json messageJson;
    messageJson["role"] = message.role;
    messageJson["content"] = message.content;

    if (!message.images.empty()) {
        messageJson["images"] = message.images;
    }

    payload["messages"] = nlohmann::json::array({messageJson});

    if (!stream) {
        payload["stream"] = false;
    }
    return payload.dump();
}