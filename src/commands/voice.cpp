#pragma once

#include <utility>
#include <curl/curl.h>
#include "../apis/apis.h"
#include "commands.h"

dpp::slashcommand play_command() {
    // create the command
    dpp::slashcommand play = dpp::slashcommand();
    play.set_name("play");
    play.set_description("Command to play audio in VC using a link or query,");
    play.add_option(dpp::command_option(dpp::co_string, "query_or_link", "Link to play or query to search.", true));
    dpp::command_option filters = dpp::command_option(dpp::co_string, "filter", "The filter to apply to the audio.",
                                                      false);
    for (auto &it: FILTERS) {
        filters.add_choice(dpp::command_option_choice(it.first, it.first));
    }
    play.add_option(filters);
    return play;
}

dpp::slashcommand join_command() {
    // create the command
    dpp::slashcommand join = dpp::slashcommand();
    join.set_name("join");
    join.set_description("Command to have Jade join the VC you are in.");
    return join;
}

dpp::slashcommand leave_command() {
    // create the command
    dpp::slashcommand leave = dpp::slashcommand();
    leave.set_name("leave");
    leave.set_description("Command to have Jade leave the VC she is currently in.");
    return leave;
}

dpp::slashcommand pause_command() {
    // create the command
    dpp::slashcommand pause = dpp::slashcommand();
    pause.set_name("pause");
    pause.set_description("Command to have Jade pause the currently playing audio.");
    return pause;
}

dpp::slashcommand resume_command() {
    // create the command
    dpp::slashcommand resume = dpp::slashcommand();
    resume.set_name("resume");
    resume.set_description("Command to have Jade resume the paused audio.");
    return resume;
}

dpp::slashcommand skip_command() {
    // create the command
    dpp::slashcommand skip = dpp::slashcommand();
    skip.set_name("skip");
    skip.set_description("Command to have Jade skip the currently playing song.");
    return skip;
}

void join_process(dpp::cluster &bot, const dpp::slashcommand_t &event) {

    event.thinking(true);
    event.edit_response("Processing your request!");

    dpp::guild *g = dpp::find_guild(event.command.guild_id);

    if (!g->connect_member_voice(event.command.get_issuing_user().id)) {
        dpp::message error_msg(event.command.channel_id,
                               "I would love to play some music, but don't you want to listen too?", dpp::mt_default);
        bot.message_create(error_msg);
        return;
    }
}

void leave_process(dpp::cluster &bot, const dpp::slashcommand_t &event) {

    dpp::voiceconn *v = event.from->get_voice(event.command.guild_id);

    if (v) {
        if (v->voiceclient->is_playing()) {
            bot.log(dpp::ll_debug, "bot is playing audio, stopping audio.");
            v->voiceclient->stop_audio();
        }
        bot.log(dpp::ll_debug, "leaving voice channel.");
        event.from->disconnect_voice(event.command.guild_id);
        event.reply("Peace out ✌\uFE0F");
    } else {
        event.reply("I'm not in a VC right now silly!");
    }
}

void pause_process(dpp::cluster &bot, const dpp::slashcommand_t &event) {

    dpp::voiceconn *v = event.from->get_voice(event.command.guild_id);

    if (!v || !v->voiceclient) {
        event.reply("I am not in a VC, nothing to pause!");
        return;
    }

    if (v->voiceclient->is_playing()) {
        v->voiceclient->pause_audio(true);
        event.reply("Paused the audio!");
    } else {
        event.reply("Nothing to pause!");
    }
}

void resume_process(dpp::cluster &bot, const dpp::slashcommand_t &event) {

    dpp::voiceconn *v = event.from->get_voice(event.command.guild_id);

    if (!v || !v->voiceclient) {
        event.reply("I am not in a VC, nothing to resume!");
        return;
    }

    if (v->voiceclient->is_paused()) {
        v->voiceclient->pause_audio(false);
        event.reply("Resumed the audio!");
    } else {
        event.reply("Nothing to resume!");
    }
}

void skip_process(dpp::cluster &bot, const dpp::slashcommand_t &event) {

    dpp::voiceconn *v = event.from->get_voice(event.command.guild_id);

    dpp::guild *guild = dpp::find_guild(event.command.guild_id);

    if (v && v->voiceclient) {
        if (v->voiceclient->is_playing()) {
            v->voiceclient->stop_audio();
            dpp::message success_msg(event.command.channel_id, "Skipped the song!", dpp::mt_default);
            event.reply(success_msg);
        } else {
            dpp::message error_msg(event.command.channel_id, "There is no song to skip!", dpp::mt_default);
            event.reply(error_msg);
        }
    } else {
        dpp::message not_in_voice_msg(event.command.channel_id, "I cannot skip a song if I am not in VC!");
        event.reply(not_in_voice_msg);
    }
}

void play_process(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link,
                  const std::string &filter) {

    event.thinking(true);

    dpp::guild *guild = dpp::find_guild(event.command.guild_id);

    if (!guild->connect_member_voice(event.command.get_issuing_user().id)) {
        dpp::message error_msg(event.command.channel_id,
                               "I would love to play some music, but don't you want to listen too?", dpp::mt_default);
        event.edit_response(error_msg);
        return;
    }

    dpp::voiceconn *channel = event.from->get_voice(event.command.guild_id);

    if (channel && channel->voiceclient && channel->voiceclient->is_ready()) {
        if (channel->voiceclient->is_playing()) {
            event.edit_response("I am playing something right now, leave me alone!");
        } else {
            bot.log(dpp::ll_debug, "Jade in VC, streaming audio.");
            event.edit_response("Processing your request!");
            stream_audio_primary(bot, event, std::move(query_or_link), filter);
        }
    } else {
        bot.log(dpp::ll_debug, "Jade not in VC, attempting to connect then stream.");
        event.edit_response("Processing your request!");
        event.from->connect_voice(guild->id, event.command.channel_id, false, true);
    }
}


void stream_audio_primary(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link,
                          const std::string &filter) {
/*
     * Function to stream audio if the bot is already connected to the VC.
     * Takes in a slashcommand_t event
     */

    if (query_or_link.empty()) {
        dpp::message no_query_or_link_msg(event.command.channel_id, "No query or link provided to stream.",
                                          dpp::mt_default);
        bot.message_create(no_query_or_link_msg);
        return;
    }

    std::string data = fmt::format(R"(yt-dlp {} -o - -f bestaudio | (ffmpeg -i pipe:0 -bufsize 50M -loglevel warning -f s16le -ac 2 -ar 48000 pipe:1))", query_or_link);
    dpp::discord_voice_client *voice_client = event.from->get_voice(event.command.guild_id)->voiceclient;

    size_t bytes_read;
    std::byte buf[11520];

    if (voice_client && voice_client->is_ready()) {

        voice_client->set_send_audio_type(dpp::discord_voice_client::satype_overlap_audio);

        // Must be "rb"!!!!! just "r" causes broken pipe on windows
        auto pipe = _popen(data.c_str(), "rb");

        while (true) {
            bytes_read = fread(buf, sizeof(std::byte), dpp::send_audio_raw_max_length, pipe);
            std::cout << "got the following amount of bytes: " << bytes_read << std::endl;
            if (bytes_read <= 0)
                    break;

            if (bytes_read <= dpp::send_audio_raw_max_length) {
                voice_client->send_audio_raw((uint16_t *) buf, sizeof(buf));
            }
        }

        voice_client->insert_marker();
        std::cout << "closing pipe like a boss!" << std::endl;
        _pclose(pipe);
    }
}

void stream_audio_secondary(dpp::cluster &bot, const dpp::voice_ready_t &event, std::string query_or_link,
                            std::uint64_t channel_id, const std::string &filter) {
    /*
     * Function to stream audio if the bot is already connected to the VC.
     * Takes in a voice_ready_t event
     * Also takes in a channel_id because we lose the channel_id
     * with this voice_ready event.
     */

    bot.log(dpp::ll_debug, "[stream_audio_secondary] -> entering the stream_audio_secondary function!");

    if (query_or_link.empty()) {
        bot.log(dpp::ll_debug, "[stream_audio_secondary] -> no query or link provided, leaving function.");
        return;
    } else {
        bot.log(dpp::ll_debug, fmt::format("[stream_audio_secondary] -> query or link of {} found.", query_or_link));
    }

    std::string data = fmt::format(
            R"(yt-dlp -f bestaudio -o - "{}" | ffmpeg -i pipe: -loglevel warning -f s16le -ac 2 -ar 48000 -acodec pcm_s16le -f wav pipe:)",
            query_or_link);

    dpp::discord_voice_client *voice_client = event.voice_client;

    size_t bytes_read;
    std::byte buf[11520];

    if (voice_client && voice_client->is_ready()) {

        voice_client->set_send_audio_type(dpp::discord_voice_client::satype_overlap_audio);

        // Must be "rb"!!!!! just "r" causes broken pipe on windows
        auto pipe = _popen(data.c_str(), "rb");

        while (true) {
            bytes_read = fread(buf, sizeof(std::byte), dpp::send_audio_raw_max_length, pipe);
            std::cout << "got the following amount of bytes: " << bytes_read << std::endl;
            if (bytes_read <= 0)
                break;

            if (bytes_read <= dpp::send_audio_raw_max_length) {
                voice_client->send_audio_raw((uint16_t *) buf, sizeof(buf));
            }
        }

        voice_client->insert_marker();
        _pclose(pipe);

        bot.log(dpp::ll_debug, "[stream_audio_secondary] -> sent all audio bytes to discord.");
    } else {
        bot.log(dpp::ll_debug, "[stream_audio_secondary] -> the voice client was null or was not ready!");
        dpp::message error_msg(channel_id, "An error has occured.", dpp::mt_default);
        bot.message_create(error_msg);
    }
}




