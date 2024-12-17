#ifndef JADE_SKIP_COMMAND_H
#define JADE_SKIP_COMMAND_H

#include <dpp/dpp.h>

dpp::slashcommand skip_command();
void skip_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

#endif
