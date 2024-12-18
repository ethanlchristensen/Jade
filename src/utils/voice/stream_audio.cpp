#include <dpp/dpp.h>
#include <fmt/format.h>
#include "utils/voice/stream_audio.h"

void stream_audio_to_discord(dpp::cluster &bot, SongRequest song)
{
    bot.log(dpp::ll_info, fmt::format("[stream_audio_to_discord]: Received a song to stream: {}, {}", song.query, song.filter));
    size_t bytes_read;
    std::byte buf[11520];

    std::string data = fmt::format(
            R"(yt-dlp -S +hdr -f bestaudio -o - "{}" | ffmpeg -i pipe: -loglevel warning -f s16le -ac 2 -ar 48000 -acodec pcm_s16le -f wav)",
            song.query);

    if (!song.filter.empty()) data += fmt::format(" -vn -filter_complex {} pipe:", song.filter);
    else data += " pipe:";

    bot.log(dpp::ll_info, "Ready to stream song to Discord: " + data);
    dpp::discord_voice_client *voice_client = song.event.from->get_voice(song.event.command.guild_id)->voiceclient;
    std::cout << "We have the voice client!\n";
    if (voice_client && voice_client->is_ready())
    {
        bot.log(dpp::ll_debug, "[stream_audio_primary] -> now playing " + song.query);
        dpp::message now_playing_message(song.event.command.channel_id, "Now playing: " + song.query,
                                         dpp::mt_default);
        bot.message_create(now_playing_message);
        voice_client->set_send_audio_type(dpp::discord_voice_client::satype_overlap_audio);

        // should be 'rb' on windows
#if defined(_WIN32) || defined(_WIN64)
        auto pipe = _popen(data.c_str(), "rb");
#else
        auto pipe = popen(data.c_str(), "r");
#endif
        // send all audio bytes from pipe to discord
        while (true)
        {
            bytes_read = fread(buf, sizeof(std::byte), dpp::send_audio_raw_max_length, pipe);
            if (bytes_read <= 0)
                break;
            if (bytes_read <= dpp::send_audio_raw_max_length)
                voice_client->send_audio_raw((uint16_t *)buf, sizeof(buf));
        }
        voice_client->insert_marker();
#if defined(_WIN32) || defined(_WIN64)
        _pclose(pipe);
#else
        pclose(pipe);
#endif
        bot.log(dpp::ll_debug, "[stream_audio_primary] -> all audio bytes send to discord.");
    }
}