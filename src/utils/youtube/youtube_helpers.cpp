#include "utils/youtube/youtube_helpers.h"

bool isValidURL(const std::string& input) {
    const std::regex urlRegex(
            R"(^(https?|ftp):\/\/[^\s/$.?#].[^\s]*)", // thanks https://github.com/nulzo/STD-Cringe/blob/cringe-2.0/src/utils/audio/cringe_yt.cpp
            std::regex::icase
    );
    return std::regex_match(input, urlRegex);
}

std::string getYoutubeUrl(const std::string& query) {
    // Format command with proper escaping and quotes
#if defined(_WIN32) || defined(_WIN64)
    std::string command = "yt-dlp.exe \"ytsearch:" + query + "\" --get-url --no-playlist -O %(webpage_url)s";
#else
    std::string command = "yt-dlp \'ytsearch:" + query + "\' --get-url --no-playlist -O \'%(webpage_url)s\'";
#endif

    std::cout << "Running command: " << command << "\n";

#if defined(_WIN32) || defined(_WIN64)
    FILE* pipe = _popen(command.c_str(), "rb");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif

    if (!pipe) {
        return "";
    }

    char buffer[4096];
    std::string url;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        url = buffer;
    }

#if defined(_WIN32) || defined(_WIN64)
    _pclose(pipe);
#else
    pclose(pipe);
#endif

    // Remove trailing whitespace and newlines
    while (!url.empty() && (url.back() == '\n' || url.back() == '\r' || url.back() == ' ')) {
        url.pop_back();
    }

    return url;
}

SongInfo getSongInfo(const std::string& url) {
    if (url.empty()) {
        return SongInfo{url, "Unknown", "Unknown", "Unknown"};
    }
    std::string command = "yt-dlp -J " + url;
    std::string jsonOutput = executeCommand(command);
    std::cout << jsonOutput << "\n";
    std::cout << "Trying to get information about the song!\n";
    try {
        nlohmann::json metadata = nlohmann::json::parse(jsonOutput);
        std::cout << "Successfully parsed the json output!\n";
        std::string title = metadata.value("title", "Unknown");
        std::cout << "Song: " << title << "\n";

        std::string artist = metadata.value("uploader", "Unknown");
        std::cout << "Artist: " << artist << "\n";

        std::string artistUrl = metadata.value("uploader_url", "Unknown");
        std::cout << "Artist Url: " << artistUrl << "\n";

        std::int32_t duration = (std::int32_t) metadata.value("duration", 0.0);
        std::cout << "Duration: " << duration << "\n";

        std::string thumbnail = metadata.value("thumbnail", "Unknown");
        std::cout << "Thumbnail: " << thumbnail << "\n";

        return SongInfo{url, title, artist, artistUrl, duration, thumbnail};
    } catch(...) {
        return SongInfo{url, "Unknown", "Unknown", "Unknown", 0, "Unknown"};
    }
}