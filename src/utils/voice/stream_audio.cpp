#include "utils/voice/stream_audio.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <functional>
#include <cstdio>

// Platform-specific pipe handlers
#if defined(_WIN32) || defined(_WIN64)
    #define POPEN_MODE "rb"
    #define POPEN _popen
    #define PCLOSE _pclose
#else
    #define POPEN_MODE "r"
    #define POPEN popen
    #define PCLOSE pclose
#endif

// RAII wrapper for pipes
class PipeGuard {
private:
    FILE* pipe_;

public:
    explicit PipeGuard(FILE* pipe) : pipe_(pipe) {}
    ~PipeGuard() {
        if (pipe_) {
            PCLOSE(pipe_);
        }
    }
    
    FILE* get() { return pipe_; }
    
    // Prevent copying
    PipeGuard(const PipeGuard&) = delete;
    PipeGuard& operator=(const PipeGuard&) = delete;
};

void stream_audio_to_discord(dpp::cluster &bot, SongRequest song, const SongInfo& songInfo)
{
    // Get voice client
    dpp::voiceconn *voice_conn = song.event.from->get_voice(song.event.command.guild_id);
    dpp::discord_voice_client *voice_client = (voice_conn) ? voice_conn->voiceclient : nullptr;

    if (voice_client == nullptr) {
        bot.log(dpp::ll_error, "Voice client is null, cannot stream audio.");
        return;
    }

    if (!voice_client->is_ready()) {
        bot.log(dpp::ll_error, "Voice client not ready, cannot stream audio.");
        return;
    }

    auto sanitize_query = [](const std::string& query) {
        std::string sanitized;
        for (char c : query) {
            if (c == '"') sanitized += "\\\"";
            else sanitized += c;
        }
        return sanitized;
    };

    std::string sanitized_query = sanitize_query(song.query);
    
    // Construct command
    std::string command = fmt::format(
        R"(yt-dlp -S +hdr -f bestaudio -o - "{}" | ffmpeg -i pipe: -loglevel warning -f s16le -ac 2 -ar 48000 -acodec pcm_s16le -f wav)",
        sanitized_query);

    command += !song.filter.empty() ? fmt::format(" -vn -filter_complex {} pipe:", song.filter) : " pipe:";

    // Log and create embed
    bot.log(dpp::ll_info, fmt::format("[stream_audio] -> now playing {}", song.query));
    dpp::embed embed = NowPlayingEmbed(song, songInfo);
    dpp::message now_playing_message(song.event.command.channel_id, embed);
    bot.message_create(now_playing_message);
    
    // Set audio type
    voice_client->set_send_audio_type(dpp::discord_voice_client::satype_overlap_audio);

    // Open pipe with RAII guard
    FILE* raw_pipe = POPEN(command.c_str(), POPEN_MODE);
    if (!raw_pipe) {
        bot.log(dpp::ll_error, fmt::format("[stream_audio] -> Failed to open pipe for command: {}", command));
        return;
    }
    
    PipeGuard pipe_guard(raw_pipe);
    
    // Stream audio
    try {
        std::byte buf[dpp::send_audio_raw_max_length];
        size_t bytes_read;

        while (!feof(raw_pipe)) {
            bytes_read = fread(buf, sizeof(std::byte), dpp::send_audio_raw_max_length, raw_pipe);
            if (bytes_read == 0) {
                if (ferror(raw_pipe)) {
                    bot.log(dpp::ll_error, "[stream_audio] -> Error reading from pipe");
                }
                break;
            }

            if (bytes_read % 4 != 0) {
                size_t aligned_size = bytes_read - (bytes_read % 4);
                bot.log(dpp::ll_debug, fmt::format("[stream_audio] -> Adjusting packet size from {} to {} bytes for alignment", bytes_read, aligned_size));
                bytes_read = aligned_size;
            }

            if (bytes_read > 0) {
                voice_client->send_audio_raw(reinterpret_cast<uint16_t*>(buf), bytes_read);
            }
        }

        voice_client->insert_marker();
        bot.log(dpp::ll_info, "[stream_audio] -> All audio bytes sent to Discord");
    }
    catch (const std::exception& e) {
        voice_client->insert_marker();
        bot.log(dpp::ll_error, fmt::format("[stream_audio] -> Exception: {}", e.what()));
    }
}