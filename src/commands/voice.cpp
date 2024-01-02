#pragma once
#include <utility>

#include "commands.h"

dpp::slashcommand play_command() {
    // create the command
    dpp::slashcommand play = dpp::slashcommand();
    play.set_name("play");
    play.set_description("Command to play audio in VC using a link or query,");
    play.add_option(dpp::command_option(dpp::co_string, "query_or_link", "Link to play or query to search.", true));
    dpp::command_option filters = dpp::command_option(dpp::co_string, "filter", "The filter to apply to the audio.", true);
    for (auto & it : FILTERS) {
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

    dpp::guild* g = dpp::find_guild(event.command.guild_id);

    if (!g->connect_member_voice(event.command.get_issuing_user().id)) {
        dpp::message error_msg(event.command.channel_id, "I would love to play some music, but don't you want to listen too?", dpp::mt_default);
        bot.message_create(error_msg);
        return;
    }

    dpp::message success_msg(event.command.channel_id, "Joined the VC!", dpp::mt_default);
    bot.message_create(success_msg);
}

void leave_process(dpp::cluster &bot, const dpp::slashcommand_t &event) {

    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

    if(v) {
        if(v->voiceclient->is_playing()) {
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

    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

    if(!v || !v->voiceclient) {
        event.reply("I am not in a VC, nothing to pause!");
        return;
    }

    if(v->voiceclient->is_playing()) {
        v->voiceclient->pause_audio(true);
        event.reply("Paused the audio!");
    } else {
        event.reply("Nothing to pause!");
    }
}

void resume_process(dpp::cluster &bot, const dpp::slashcommand_t &event) {

    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

    if(!v || !v->voiceclient) {
        event.reply("I am not in a VC, nothing to resume!");
        return;
    }

    if(v->voiceclient->is_paused()) {
        v->voiceclient->pause_audio(false);
        event.reply("Resumed the audio!");
    } else {
        event.reply("Nothing to resume!");
    }
}

void skip_process(dpp::cluster &bot, const dpp::slashcommand_t &event) {

    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

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

void play_process(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link, const std::string& filter) {

    event.thinking(true);

    dpp::guild *guild = dpp::find_guild(event.command.guild_id);

    if (!guild->connect_member_voice(event.command.get_issuing_user().id)) {
        dpp::message error_msg(event.command.channel_id, "I would love to play some music, but don't you want to listen too?", dpp::mt_default);
        event.edit_response(error_msg);
        return;
    }

    dpp::voiceconn* channel = event.from->get_voice(event.command.guild_id);

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

void stream_audio_primary(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link, const std::string& filter) {
/*
     * Function to stream audio if the bot is already connected to the VC.
     * Takes in a slashcommand_t event
     */

    std::vector<uint8_t> pcmdata;
    mpg123_init();

    int err = 0;
    unsigned char* buffer;
    size_t buffer_size, done;
    int channels, encoding;
    long rate;

    /* Note it is important to force the frequency to 48000 for Discord compatibility */
    mpg123_handle *mh = mpg123_new(nullptr, &err);
    mpg123_param(mh, MPG123_FORCE_RATE, 48000, 48000.0);

    buffer_size = mpg123_outblock(mh);
    buffer = new unsigned char[buffer_size];

    bot.log(dpp::ll_debug, fmt::format("[stream_audio_primary] -> user sent in -> {}", query_or_link));
    std::string full_download_command = fmt::format("{} {}", MP3DOWNLOAD, query_or_link);

    bot.log(dpp::ll_debug, "[stream_audio_primary] -> downloading the audio.");
    std::system(full_download_command.c_str());
    bot.log(dpp::ll_debug, "[stream_audio_primary] -> finished downloading the audio.");

    mpg123_open(mh, MUSIC_FILE);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    unsigned int counter = 0;
    for (int totalBytes = 0; mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK; ) {
        for (size_t i = 0; i < buffer_size; i++) {
            pcmdata.push_back(buffer[i]);
        }
        counter += buffer_size;
        totalBytes += done;
    }
    delete[] buffer;
    mpg123_close(mh);
    mpg123_delete(mh);

    dpp::voiceconn* channel = event.from->get_voice(event.command.guild_id);

    if (channel && channel->voiceclient && channel->voiceclient->is_ready()) {
        bot.log(dpp::ll_debug, "[stream_audio_primary] -> attempting to send the now_playing_msg.");
        dpp::message now_playing_msg(event.command.channel_id, fmt::format("Now playing: {}", query_or_link),dpp::mt_default);
        bot.message_create(now_playing_msg);
        channel->voiceclient->send_audio_raw((uint16_t *) pcmdata.data(), pcmdata.size());
    }
    else {
        bot.log(dpp::ll_debug, "[stream_audio_primary] -> attempting to send the error_msg");
        dpp::message now_playing_msg(event.command.channel_id, "I ran into an error.",dpp::mt_default);
        bot.message_create(now_playing_msg);
    }
}

void stream_audio_secondary(dpp::cluster &bot, const dpp::voice_ready_t &event, std::string query_or_link, std::uint64_t channel_id, const std::string& filter) {
    /*
     * Function to stream audio if the bot is already connected to the VC.
     * Takes in a voice_ready_t event
     * Also takes in a channel_id because we lose the channel_id
     * with this voice_ready event.
     */

    bot.log(dpp::ll_debug, "[stream_audio_secondary] -> entering the stream_audio_secondary function!");

    if (query_or_link.empty()) {
        dpp::message no_query_or_link_msg(channel_id, "No query or link provided to stream.", dpp::mt_default);
        bot.message_create(no_query_or_link_msg);
        return;
    }

    std::vector<uint8_t> pcmdata;
    mpg123_init();

    int err = 0;
    unsigned char* buffer;
    size_t buffer_size, done;
    int channels, encoding;
    long rate;

    /* Note it is important to force the frequency to 48000 for Discord compatibility */
    mpg123_handle *mh = mpg123_new(nullptr, &err);
    mpg123_param(mh, MPG123_FORCE_RATE, 48000, 48000.0);

    buffer_size = mpg123_outblock(mh);
    buffer = new unsigned char[buffer_size];


    bot.log(dpp::ll_debug, fmt::format("[stream_audio_secondary] -> user sent in -> {}", query_or_link));
    std::string full_download_command = fmt::format("{} {}", MP3DOWNLOAD, query_or_link);

    bot.log(dpp::ll_debug, "[stream_audio_secondary] -> downloading the audio.");
    std::system(full_download_command.c_str());
    bot.log(dpp::ll_debug, "[stream_audio_secondary] -> finished downloading the audio.");

    mpg123_open(mh, MUSIC_FILE);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    unsigned int counter = 0;
    for (int totalBytes = 0; mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK; ) {
        for (size_t i = 0; i < buffer_size; i++) {
            pcmdata.push_back(buffer[i]);
        }
        counter += buffer_size;
        totalBytes += done;
    }
    delete[] buffer;
    mpg123_close(mh);
    mpg123_delete(mh);


    dpp::discord_voice_client * voice_client = event.voice_client;

    if (voice_client && voice_client->is_ready()) {
        bot.log(dpp::ll_debug, "[stream_audio_secondary] -> attempting to send the 'now_playing_msg'.");
        dpp::message now_playing_msg(channel_id, fmt::format("Now playing: {}", query_or_link),dpp::mt_default);
        bot.message_create(now_playing_msg);
        voice_client->send_audio_raw((uint16_t *) pcmdata.data(), pcmdata.size());
    }
    else {
        bot.log(dpp::ll_debug, "[stream_audio_secondary] -> attempting to send the 'error_msg'.");
        dpp::message error_msg(channel_id, "I ran into an error.",dpp::mt_default);
        bot.message_create(error_msg);
    }
}




