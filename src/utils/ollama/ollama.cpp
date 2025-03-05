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

std::string
OllamaAPI::checkImage(const std::string& model, const std::string& imageData) {
    // Convert image data to base64
    std::string base64Image = encode_to_base64(imageData);

    ChatMessage message;
    message.role = "user";
    message.content = "Is this image appropriate for all audiences? Please analyze this image and determine if it contains any inappropriate content such as adult material, violence, gore, hate speech, or other NSFW elements. Respond with 'INAPPROPRIATE' if the image is unsuitable, or 'APPROPRIATE' if the image is safe for all audiences.";
    message.images.push_back(base64Image);

    std::string response = sendMessage(model, message, false);

    try {
        nlohmann::json response_json = nlohmann::json::parse(response);
        return response_json["message"]["content"];
    } catch (const std::exception& e) {
        return "Error analyzing image: " + std::string(e.what());
    }
}