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
void chat_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken);
void summarize_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken);
void extract_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken);
void play_process(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link);
void join_process(dpp::cluster &bot, const dpp::slashcommand_t &event);
void leave_process(dpp::cluster &bot, const dpp::slashcommand_t &event);
void pause_process(dpp::cluster &bot, const dpp::slashcommand_t &event);
void resume_process(dpp::cluster &bot, const dpp::slashcommand_t &event);
void skip_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void stream_audio_primary(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link);
void stream_audio_secondary(dpp::cluster &bot, const dpp::voice_ready_t &event, std::string query_or_link, dpp::slashcommand_t &previous_play_event);
