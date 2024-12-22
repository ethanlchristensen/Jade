#ifndef JADE_LEAVE_COMMAND_H
#define JADE_LEAVE_COMMAND_H

#include <dpp/dpp.h>

dpp::slashcommand leave_command();
void leave_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

#endif //JADE_LEAVE_COMMAND_H
