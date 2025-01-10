#ifndef JADE_DESCRIBE_COMMAND_H
#define JADE_DESCRIBE_COMMAND_H


#include <thread>
#include <sstream>
#include <iostream>
#include <dpp/dpp.h>
#include "utils/ollama/ollama.h"
#include "utils/jade_embed.h"
#include "utils/jade_util.h"

dpp::slashcommand describe_command();
void describe_process(const dpp::cluster &bot, const dpp::slashcommand_t& event, OllamaAPI &ollamaApi);

#endif //JADE_DESCRIBE_COMMAND_H
