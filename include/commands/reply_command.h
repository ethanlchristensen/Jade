#ifndef REPLY_COMMAND_H
#define REPLY_COMMAND_H

#include <dpp/dpp.h>

#include "utils/jade_util.h"

dpp::slashcommand reply_command();
void reply_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

#endif //REPLY_COMMAND_H
