#include "utils/gemini/gemini.h"

GeminiAPI::GeminiAPI(std::string baseURL, std::string apiKey)
        : baseUrl(std::move(baseURL)), apiKey(std::move(apiKey)) {
}

std::string
GeminiAPI::sendMessage(const std::string& model, const std::string& role,
                       const std::string& content, bool stream) {
    ChatMessage message{role, content, {}};
    return sendMessage(model, message, stream);
}

std::string
GeminiAPI::sendMessage(const std::string& model, const ChatMessage& message,
                       bool stream) {
    std::string endpoint = baseUrl + "/v1beta/models/" + model + ":generateContent?key=" + apiKey;
    std::string data = constructPayload(model, message, stream);
    std::vector<std::string> headers = {"Content-Type: application/json"};
    return APIClient::POST(endpoint, data, "", headers);
}

std::string
GeminiAPI::constructPayload(const std::string& model, const std::string& role,
                            const std::string& content, bool stream) {
    ChatMessage message{role, content, {}};
    return constructPayload(model, message, stream);
}

std::string
GeminiAPI::constructPayload(const std::string& model, const ChatMessage& message,
                            bool stream) {
    nlohmann::json payload;

    nlohmann::json contents = nlohmann::json::array();
    nlohmann::json messageJson;

    // Format role for Gemini (user vs model)
    messageJson["role"] = message.role == "system" || message.role == "assistant" ? "model" : "user";

    // Create parts array for content and images
    nlohmann::json parts = nlohmann::json::array();

    // Add text content if present
    if (!message.content.empty()) {
        nlohmann::json textPart;
        textPart["text"] = message.content;
        parts.push_back(textPart);
    }

    // Add images if present
    for (const auto& image : message.images) {
        nlohmann::json imagePart;
        nlohmann::json inlineData;
        inlineData["mime_type"] = "image/jpeg";
        inlineData["data"] = image;
        imagePart["inline_data"] = inlineData;
        parts.push_back(imagePart);
    }

    messageJson["parts"] = parts;
    contents.push_back(messageJson);

    payload["contents"] = contents;

    // Add stream parameter if requested
    if (stream) {
        payload["stream"] = true;
    }

    return payload.dump();
}

std::string
GeminiAPI::checkImage(const std::string& model, const std::string& imageData) {
    // Convert image data to base64
    std::string base64Image = encode_to_base64(imageData);

    ChatMessage message;
    message.role = "user";
    message.content = "Is this image appropriate for all audiences? Please analyze this image and determine if it contains any inappropriate content such as adult material, violence, gore, hate speech, or other NSFW elements. Also check if that image may contain API keys. Respond with 'INAPPROPRIATE' if the image is unsuitable, or 'APPROPRIATE' if the image is safe for all audiences.";
    message.images.push_back(base64Image);

    std::string response = sendMessage(model, message, false);

    try {
        nlohmann::json response_json = nlohmann::json::parse(response);

        // Extract text from Gemini response format
        if (response_json.contains("candidates") &&
            !response_json["candidates"].empty() &&
            response_json["candidates"][0].contains("content") &&
            response_json["candidates"][0]["content"].contains("parts") &&
            !response_json["candidates"][0]["content"]["parts"].empty() &&
            response_json["candidates"][0]["content"]["parts"][0].contains("text")) {

            return response_json["candidates"][0]["content"]["parts"][0]["text"];
        }
        std::cout << response_json << "\n";
        return "Error parsing Gemini response";
    } catch (const std::exception& e) {
        return "Error analyzing image: " + std::string(e.what());
    }
}