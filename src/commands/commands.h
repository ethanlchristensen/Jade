#include <dpp/dpp.h>
#include <fmt/format.h>
#include <iomanip>
#include <sstream>
#include <regex>

#include <vector>
#include <fstream>
#include <iostream>
#include <mpg123.h>
#include <out123.h>

#define MUSIC_FILE "C:/Users/ethan/Github/Jade/src/commands/audio/audio.mp3"
#define MP3DOWNLOAD R"(youtube-dl --user-agent "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36" --extract-audio --audio-format mp3 -o "C:/Users/ethan/Github/Jade/src/commands/audio/audio.%(ext)s")"

<<<<<<< Updated upstream
=======
static std::map<std::string, std::string> FILTERS = {
        {"sigma",                                "aresample=48000, asetrate=48000*0.8,bass=g=13:f=110:w=0.6"},
        {"nightcore",                            "aresample=48000, asetrate=48000*1.5"},
        {"pulsar",                               "apulsator=amount=1:width=2"},
        {"earrape",                              "acrusher=level_in=8:level_out=18:bits=8:mode=log:aa=1"},
        {"bassboost",                            "highpass=f=50, lowpass=f=2750, aresample=48000, bass=g=15, apsyclip=level_in=2:level_out=2"},
        {"nuclear",                              "apsyclip=level_in=64:level_out=64:clip=1"},
        {"softclip",                             "asoftclip=hard:output=1"},
        {"psyclip",                              "apsyclip=level_in=2:level_out=2, bass=f=110:w=1"},
        {"reverb",                               "aecho=1.0:1.0:50:0.5"},
        {"slowedandreverb",                      "aecho=1.0:0.7:30:0.5, aresample=48000, asetrate=48000*0.85, compand=attacks=0:points=-80/-169|-54/-80|-49.5/-64.6|-41.1/-41.1|-25.8/-15|-10.8/-4.5|0/0|20/8.3"},
        {"lowpass",                              "acrossover=4500:order=20th[k][r];[r]anullsink;[k]anull"},
        {"none",                                 "aresample=48000"},
        {"POVUrGfBangsKlimWhileUrInTheBathroom", "highpass=f=10, lowpass=f=400, aresample=44100, asetrate=44100*0.85,bass=g=4:f=110:w=0.6, alimiter=1, compand=attacks=0:points=-80/-169|-54/-80|-49.5/-64.6|-41.1/-41.1|-25.8/-15|-10.8/-4.5|0/0|20/8.3"},
        {"vaporwave",                            "highpass=f=50, lowpass=f=2750, aresample=48000, asetrate=48000*0.85,bass=g=5:f=110:w=0.6, compand=attacks=0:points=-80/-169|-54/-80|-49.5/-64.6|-41.1/-41.1|-25.8/-15|-10.8/-4.5|0/0|20/8.3"},
        {"slowwwwww",                            "acrusher=level_in=2:level_out=5, asetrate=48000*0.7, bass=g=16:f=120:w=0.6"},
        {"wide",                                 "extrastereo, afireqsrc=p='clear', stereowiden, earwax"}
};

>>>>>>> Stashed changes
dpp::slashcommand nolan_command();

dpp::slashcommand clear_command();

dpp::slashcommand echo_command();

dpp::slashcommand chat_command();

dpp::slashcommand summarize_command();

dpp::slashcommand extract_command();

dpp::slashcommand play_command();

dpp::slashcommand join_command();

dpp::slashcommand leave_command();

dpp::slashcommand pause_command();

dpp::slashcommand resume_command();

dpp::slashcommand skip_command();

void nolan_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void clear_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void echo_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void chat_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string &apiToken);

void summarize_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string &apiToken);

void extract_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string &apiToken);

void play_process(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link, std::string filter);

void join_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void leave_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void pause_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void resume_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void skip_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

<<<<<<< Updated upstream
void stream_audio_primary(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link);
void stream_audio_secondary(dpp::cluster &bot, const dpp::voice_ready_t &event, std::string query_or_link, dpp::slashcommand_t &previous_play_event);
=======
void stream_audio_primary(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link, const std::string& filter);

void stream_audio_secondary(dpp::cluster &bot, const dpp::voice_ready_t &event, std::string query_or_link,
                            std::uint64_t channel_id, const std::string& filter);
>>>>>>> Stashed changes
