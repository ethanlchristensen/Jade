//
// Created by ethan on 12/16/2024.
//
#include <iostream>
#include <dpp/dpp.h>
#include <fmt/format.h>
#include "utils/voice/stream_audio.h"

void stream_audio_to_discord(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link,
                          const std::string &filter)
{
    /*
     * Function to stream audio if the bot is already connected to the VC.
     */
    size_t bytes_read;
    std::byte buf[11520];

    bot.log(dpp::ll_debug, "[stream_audio_primary] -> entering the stream_audio_primaryfunction.");
    if (query_or_link.empty())
    {
        bot.log(dpp::ll_debug, "[stream_audio_primary] -> no query or link provided, leaving function.");
        dpp::message no_query_or_link_msg(event.command.channel_id, "No query or link provided to stream.",
                                          dpp::mt_default);
        bot.message_create(no_query_or_link_msg);
        return;
    }
    std::string data = fmt::format(
            R"(yt-dlp -S +hdr -f bestaudio -o - "{}" | ffmpeg -i pipe: -loglevel warning -f s16le -ac 2 -ar 48000 -acodec pcm_s16le -f wav)",
            query_or_link);

    if (!filter.empty()) data += fmt::format(" -vn -filter_complex {} pipe:", filter);
    else data += " pipe:";

    dpp::discord_voice_client *voice_client = event.from->get_voice(event.command.guild_id)->voiceclient;
    if (voice_client && voice_client->is_ready())
    {
        bot.log(dpp::ll_debug, "[stream_audio_primary] -> now playing " + query_or_link);
        dpp::message now_playing_message(event.command.channel_id, "Now playing: " + query_or_link,
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

void stream_audio_secondary(dpp::cluster &bot, const dpp::voice_ready_t &event, std::string query_or_link,
                            std::uint64_t channel_id, const std::string &filter)
{
    /*
     * Function to stream audio if the bot is already connected to the VC.
     */
    size_t bytes_read;
    std::byte buf[11520];

    bot.log(dpp::ll_debug, "[stream_audio_secondary] -> entering the stream_audio_secondary function.");
    if (query_or_link.empty())
    {
        bot.log(dpp::ll_debug, "[stream_audio_secondary] -> no query or link provided, leaving function.");
        dpp::message no_query_or_link_msg(channel_id, "No query or link provided to stream.",
                                          dpp::mt_default);
        bot.message_create(no_query_or_link_msg);
        return;
    }

    std::string data = fmt::format(
            R"(yt-dlp -S +hdr -f bestaudio -o - "{}" | ffmpeg -i pipe: -loglevel warning -f s16le -ac 2 -ar 48000 -acodec pcm_s16le -f wav)",
            query_or_link);

    if (!filter.empty()) data += fmt::format(" -vn -filter_complex {} pipe:", filter);
    else data += " pipe:";

    dpp::discord_voice_client *voice_client = event.voice_client;
    if (voice_client && voice_client->is_ready())
    {
        bot.log(dpp::ll_debug, "[stream_audio_secondary] -> now playing " + query_or_link);
        dpp::message now_playing_message(channel_id, "Now playing: " + query_or_link,
                                         dpp::mt_default);
        bot.message_create(now_playing_message);
        voice_client->set_send_audio_type(dpp::discord_voice_client::satype_overlap_audio);
        // should be 'rb' on windows
#if defined(_WIN32) || defined(_WIN64)
        auto pipe = _popen(data.c_str(), "rb");
#else
        auto pipe = popen(data.c_str(), "r");
#endif
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
        bot.log(dpp::ll_debug, "[stream_audio_secondary] -> all audio bytes to discord.");
    }
    else
        bot.log(dpp::ll_debug, "[stream_audio_secondary] -> the voice client was null or was not ready.");
}
