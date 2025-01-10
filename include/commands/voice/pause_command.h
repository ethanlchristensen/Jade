#ifndef JADE_PAUSE_COMMAND_H
#define JADE_PAUSE_COMMAND_H

#include <dpp/dpp.h>

dpp::slashcommand pause_command();
void pause_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

#endif
