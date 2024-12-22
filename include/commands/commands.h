#include <dpp/dpp.h>
#include <fmt/format.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

#include <dpp/dpp.h>
#include <fmt/format.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <mpg123.h>
#include <opus/opus.h>
#include <out123.h>


dpp::slashcommand nolan_command();

dpp::slashcommand clear_command();

dpp::slashcommand echo_command();

dpp::slashcommand summarize_command();

dpp::slashcommand extract_command();

dpp::slashcommand say_command();

void nolan_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void clear_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

void echo_process(dpp::cluster &bot, const dpp::slashcommand_t &event);
