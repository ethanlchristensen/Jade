#include "utils/jade_util.h"
#include <algorithm>
#include <chrono>


#ifdef _WIN32

inline FILE *platform_popen(const char *command, const char *mode) {
    return _popen(command, mode);
}

inline int platform_pclose(FILE *stream) {
    return _pclose(stream);
}

#else
inline FILE* platform_popen(const char* command, const char* mode) {
    return popen(command, mode);
}
inline int platform_pclose(FILE* stream) {
    return pclose(stream);
}
#endif


std::vector<std::string> devMessages = {"Engines warming up... Dev in progress.", "Stars aligning. Dev mode active!",
                                        "Coding an update!", "Building the next mission!",
                                        "Navigating dev space. Stay tuned!", "In dev mode. Prepare for lift-off!",
                                        "Systems upgrading. Dev phase on!", "Cosmic code in the works!",
                                        "Core systems in dev!", "Dev mode engaged. Updates soon!",
                                        "Crafting the future in dev!", "Cosmic engine in beta. Hold tight!",
                                        "Exploring deep code. Stay tuned!", "In the lab, refining things.",
                                        "Countdown to release. Dev in progress!", "Coding at light speed!",
                                        "Mission control: In dev.", "Future in the lab!",
                                        "Cosmic experiments in progress!", "In dev mode. Stay tuned!"};


std::vector<std::string> prodMessages = {"Ready for launch!", "Systems online. Live!", "Mission complete. Operational!",
                                         "Engines powered. At service!", "Live and ready!", "All systems go. Live!",
                                         "Mission active!", "Ready for the stars. In orbit!", "Operational!",
                                         "In full swing!", "Entered the cosmos!", "Mission success! On standby.",
                                         "Launched. Let’s go!", "All systems functioning. Live!",
                                         "Fully operational. Let’s go!", "At command. Ready for mission!",
                                         "Online, exploring stars!", "Lift-off complete. Live!", "Code’s solid. Ready!",
                                         "Operational and engaged!"};


std::string executeCommand(const std::string &command) {
    std::array<char, 128> buffer{};
    std::string result;
    FILE *pipe = platform_popen(command.c_str(), "r");

    if (!pipe) {
        throw std::runtime_error("failed to open the pipe!");
    }
    try {
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            result.append(buffer.data());
            buffer.fill(0);  // Clear the buffer after each use
        }
    } catch (...) {
        platform_pclose(pipe);
        throw;
    }
    platform_pclose(pipe);
    std::string finalResult = std::move(result);  // Ensure complete move of data
    return finalResult;
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

std::string APIClient::POST(const std::string &url, const std::string &data, const std::string &authToken,
                            const std::vector<std::string> &additionalHeaders) {
    std::cout << "POST called!\n";
    CURL *curl = curl_easy_init();
    if (!curl) {
        return "Error initializing cURL";
    }

    std::cout << "[POST] " + url + "\n";

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    struct curl_slist *headers = nullptr;

    // Add auth token if provided
    if (!authToken.empty()) {
        std::string bearerToken = "Authorization: Bearer " + authToken;
        headers = curl_slist_append(headers, bearerToken.c_str());
    }

    // Add any additional headers
    for (const auto &header: additionalHeaders) {
        headers = curl_slist_append(headers, header.c_str());
    }

    // Set headers if any were added
    if (headers) {
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

size_t APIClient::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *out) {
    size_t totalSize = size * nmemb;
    out->append((char *) contents, totalSize);
    return totalSize;
}

size_t APIClient::WriteCallbackToVector(void *contents, size_t size, size_t nmemb, std::vector<char> *out) {
    size_t totalSize = size * nmemb;
    out->insert(out->end(), (char *) contents, (char *) contents + totalSize);
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

std::string encode_to_base64(const std::string &data) {
#ifdef _WIN32
    return base64::encode(data);
#else
    base64_encodestate state;
    base64_init_encodestate(&state);

    std::vector<char> output(data.size() * 2);  // Allocate buffer for output
    int count = base64_encode_block(data.c_str(), data.size(), output.data(), &state);
    count += base64_encode_blockend(output.data() + count, &state);

    return std::string(output.data(), count);
#endif
}

std::string extractFirstFrameFromVideo(const std::string& videoUrl) {
    // Create a temporary file for the video/gif with a unique name in the current directory
    std::string tempVideoPath = "temp_media_" + std::to_string(time(nullptr));
    std::string tempFramePath = "temp_frame_" + std::to_string(time(nullptr)) + ".jpg";

    try {
        // Download the video/gif (keeping all query parameters)
        int downloadResult = std::system(("curl -s -L -o \"" + tempVideoPath + "\" \"" + videoUrl + "\"").c_str());
        if (downloadResult != 0) {
            throw std::runtime_error("Failed to download media file");
        }

        // Check if the file exists
        std::ifstream mediaFile(tempVideoPath);
        if (!mediaFile.good()) {
            throw std::runtime_error("Media file was not created or is not accessible");
        }
        mediaFile.close();

        // Extract the first frame using FFmpeg (works for both videos and GIFs)
        int ffmpegResult = std::system(("ffmpeg -i \"" + tempVideoPath + "\" -vframes 1 \"" + tempFramePath + "\" -y").c_str());
        if (ffmpegResult != 0) {
            throw std::runtime_error("FFmpeg failed to extract frame from media file");
        }

        // Check if the frame file exists
        std::ifstream frameFile(tempFramePath, std::ios::binary);
        if (!frameFile.good()) {
            throw std::runtime_error("Frame file was not created or is not accessible");
        }

        // Read the frame into a string
        std::string image_data((std::istreambuf_iterator<char>(frameFile)), std::istreambuf_iterator<char>());
        frameFile.close();

        // Clean up temporary files
        std::remove(tempVideoPath.c_str());
        std::remove(tempFramePath.c_str());

        return image_data;
    } catch (const std::exception& e) {
        // Log the error
        std::cerr << "Error processing media: " << e.what() << std::endl;

        // Clean up any temporary files that might have been created
        std::remove(tempVideoPath.c_str());
        std::remove(tempFramePath.c_str());

        // Return an empty string to indicate failure
        return "";
    }
}
