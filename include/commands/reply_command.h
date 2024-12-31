#ifndef REPLY_COMMAND_H
#define REPLY_COMMAND_H

#include <dpp/dpp.h>

dpp::slashcommand reply_command();
void reply_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

#endif //REPLY_COMMAND_H
