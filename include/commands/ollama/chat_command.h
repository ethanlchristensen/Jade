#ifndef JADE_CHAT_COMMAND_H
#define JADE_CHAT_COMMAND_H

#include <thread>
#include <iostream>
#include <dpp/dpp.h>
#include "utils/ollama/ollama.h"
#include "utils/jade_embed.h"

static std::map<std::string, std::string> MODELS = {
        {"Jade",   "Jade"},
        {"Marcus", "Marcus"}
};

dpp::slashcommand chat_command();
void chat_process(dpp::cluster &bot, const dpp::slashcommand_t& event, std::string& message, std::string& model, OllamaAPI &ollamaApi);

#endif //JADE_CHAT_COMMAND_H