#include <dpp/dpp.h>
#include <iostream>

dpp::slashcommand nolan_command();
dpp::slashcommand clear_command();

void nolan_process(dpp::cluster &bot, const dpp::slashcommand_t &event);
void clear_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

