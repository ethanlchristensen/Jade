#ifndef JADE_JOIN_COMMAND_H
#define JADE_JOIN_COMMAND_H

#include <dpp/dpp.h>

dpp::slashcommand join_command();
void join_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

#endif //JADE_JOIN_COMMAND_H
