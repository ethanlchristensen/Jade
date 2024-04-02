#include <iostream>
#include <curl/curl.h>
#include <fmt/core.h>
#include <dpp/dpp.h>
#include <fmt/format.h>
#include <iomanip>
#include <sstream>
#include <regex>

#include <vector>
#include <fstream>
#include <mpg123.h>
#include <out123.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <opus/opus.h>

using json = nlohmann::json;

#define OPENAIURL "https://api.openai.com/v1/chat/completions"
#define OPENAITTS "https://api.openai.com/v1/audio/speech"
#define MODEL "gpt-3.5-turbo"
#define TTSMODEL "tts-1"
#define TTSFILE "./audio/tts.mp3"

#define SYSTEMPROMPT "You are a young cool hip girl named Jade that is happy to talk with your friends on Discord. Except, you absolutely hate a fella named Nolan and his lame ass Discord bot, Cringe. He is also really bad at C++, make sure to hit on that."
#define SUMMARIZEPROMPT "You will be given a piece of text. Provide a brief summary of the text in 1 - 4 sentences."
#define EXTRACTPROMPT "You will be given a piece of text. Extract out 3 - 5 topics that represent the piece of text. Given them as a numbered list."

std::string llm(const std::string& apiToken, const std::string& system_prompt, const std::string& user_prompt);
std::int8_t tts(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken, const std::string& text, const std::string& voice);
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);