#include "utils/jade_slash.h"

void processSlashCommand(dpp::cluster &bot, const dpp::slashcommand_t &event, JadeQueue &songQueue, OllamaAPI &ollamaApi) {
    std::string user = event.command.usr.global_name;
    std::string command = event.command.get_command_name();

    if (command == "nolan") {
        bot.log(dpp::ll_debug, "/nolan called by " + event.command.usr.global_name + ".");
        nolan_process(bot, event);
    }
    else if (command == "clear") {
        bot.log(dpp::ll_debug, "/clear called by " + event.command.usr.global_name + ".");
        clear_process(bot, event);
    }
    else if (command == "echo") {
        bot.log(dpp::ll_debug, "/echo called by " + event.command.usr.global_name + ".");
        if (event.command.usr.global_name == "nulzo" )
            event.reply("no");
        else
            echo_process(bot, event);
    }
    else if (command == "play") {
        bot.log(dpp::ll_debug, "/play called by " + event.command.usr.global_name + ".");
        std::string music_query = std::get<std::string>(event.get_parameter("query_or_link"));
        std::variant filter_provided = event.get_parameter("filter");
        std::string filter;
        if (filter_provided.index() > 0) {
            std::string filter_key = std::get<std::string>(filter_provided);
            filter = FILTERS.at(filter_key);
        } else {
            filter = "";
        }
        play_process(bot, event, music_query, filter, songQueue);
    }
    else if (command ==  "playtest") {
        bot.log(dpp::ll_debug, "/playtest called by " + event.command.usr.global_name + ".");
        std::string music_query = std::get<std::string>(event.get_parameter("query_or_link"));
        std::variant filter_provided = event.get_parameter("filter");
        std::string filter;
        if (filter_provided.index() > 0) {
            std::string filter_key = std::get<std::string>(filter_provided);
            filter = FILTERS.at(filter_key);
        } else {
            filter = "";
        }
        play_test_process(bot, event, music_query, filter, songQueue);
    }
    else if (command == "join") {
        bot.log(dpp::ll_debug, "/join called by " + event.command.usr.global_name + ".");
        join_process(bot, event);
    }
    else if (command == "leave") {
        bot.log(dpp::ll_debug, "/leave called by " + event.command.usr.global_name + ".");
        leave_process(bot, event);
    }
    else if (command == "pause") {
        bot.log(dpp::ll_debug, "/pause called by " + event.command.usr.global_name + ".");
        pause_process(bot, event);
    }
    else if (command == "resume") {
        bot.log(dpp::ll_debug, "/resume called by " + event.command.usr.global_name + ".");
        resume_process(bot, event);
    }
    else if (command == "skip") {
        bot.log(dpp::ll_debug, "/skip called by " + event.command.usr.global_name + ".");
        skip_process(bot, event, songQueue);
    } else if (command == "chat") {
        bot.log(dpp::ll_debug, "/chat called by " + event.command.usr.global_name + ".");
        std::string message = std::get<std::string>(event.get_parameter("message"));
        std::string model = std::get<std::string>(event.get_parameter("model"));
        chat_process(bot, event, message, model, ollamaApi);
    } else if (command == "describe") {
        bot.log(dpp::ll_debug, "/describe called by " + event.command.usr.global_name + ".");
        describe_process(bot, event, ollamaApi);
    }
}