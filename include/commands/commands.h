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
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <opus/opus.h>

#define MUSIC_FILE "C:/Users/ethan/Desktop/Github/Jade/src/commands/audio/audio.mp3"
#define MP3DOWNLOAD R"(youtube-dl --user-agent "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36" --extract-audio --audio-format mp3 -o "C:/Users/ethan/Desktop/Github/Jade/src/commands/audio/audio.%(ext)s")"

static std::map<std::string, std::string> FILTERS = {
        {"VAPORWAVE", "\"highpass=f=50, lowpass=f=2750, aresample=48000, asetrate=48000*0.85,bass=g=5:f=110:w=0.6, compand=attacks=0:points=-80/-169|-54/-80|-49.5/-64.6|-41.1/-41.1|-25.8/-15|-10.8/-4.5|0/0|20/8.3\""},
        {"INTHEBATHROOM", "\"highpass=f=10, lowpass=f=400, aresample=44100, asetrate=44100*0.85,bass=g=4:f=110:w=0.6, alimiter=1, compand=attacks=0:points=-80/-169|-54/-80|-49.5/-64.6|-41.1/-41.1|-25.8/-15|-10.8/-4.5|0/0|20/8.3\""},
        {"NIGHTCORE", "\"aresample=48000, asetrate=48000*1.5\""},
        {"BASSBOOST", "\"bass=g=20:f=110:w=0.3\""},
        {"DIM", "apulsator=hz=0.09"},
        {"LOFI", "aresample=48000,asetrate=48000*0.9,extrastereo=m=2.5:c=disabled"},
        {"NOSILENCE", "\"silenceremove=1:0:-50dB\""},
        {"EXPANDER", "\"compand=attacks=0:points=-80/-169|-54/-80|-49.5/-64.6|-41.1/-41.1|-25.8/-15|-10.8/-4.5|0/0|20/8.3\""}
};

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

dpp::slashcommand say_command();

void nolan_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void clear_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void echo_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void chat_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string &apiToken);

void summarize_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string &apiToken);

void extract_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string &apiToken);

void play_process(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link, const std::string& filter);

void join_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void leave_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void pause_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void resume_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void skip_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void stream_audio_primary(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link, const std::string& filter);

void stream_audio_secondary(dpp::cluster &bot, const dpp::voice_ready_t &event, std::string query_or_link, std::uint64_t channel_id, const std::string& filter);

void say_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string &apiToken);
