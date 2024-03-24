#include "commands/commands.h"
#include "apis/apis.h"

dpp::slashcommand say_command()
{
    // create the slash command
    dpp::slashcommand say = dpp::slashcommand();
    say.set_name("say");
    say.set_description("Command to have Jade say in a VC.");
    say.add_option(dpp::command_option(dpp::co_string, "message", "The message to be said.", true));
    say.add_option(
        dpp::command_option(dpp::co_string, "voice", "The voice you want Jade to speak in.", true)
            .add_choice(dpp::command_option_choice("alloy", std::string("alloy")))
            .add_choice(dpp::command_option_choice("nova", std::string("nova")))
            .add_choice(dpp::command_option_choice("shimmer", std::string("shimmer")))
            .add_choice(dpp::command_option_choice("echo", std::string("echo")))
            .add_choice(dpp::command_option_choice("fable", std::string("fable")))
            .add_choice(dpp::command_option_choice("onyx", std::string("onyx"))));
    return say;
}

void say_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string &apiToken)
{

    std::int8_t res;

    event.thinking(true);

    dpp::guild *guild = dpp::find_guild(event.command.guild_id);

    if (!guild->connect_member_voice(event.command.get_issuing_user().id))
    {
        dpp::message error_msg(event.command.channel_id, "I would love to play some music, but don't you want to listen too?", dpp::mt_default);
        event.edit_response(error_msg);
        return;
    }

    std::string say_message = std::get<std::string>(event.get_parameter("message"));

    std::replace(say_message.begin(), say_message.end(), '"', '\'');

    std::string say_voice = std::get<std::string>(event.get_parameter("voice"));

    dpp::voiceconn *channel = event.from->get_voice(event.command.guild_id);

    if (channel && channel->voiceclient && channel->voiceclient->is_ready())
    {
        if (channel->voiceclient->is_playing())
        {
            event.edit_response("I am playing something right now, leave me alone!");
        }
        else
        {
            bot.log(dpp::ll_debug, "[say_process] -> Jade is in a VC, attempting to download and play tts.");
            event.edit_response("Processing your request!");
            res = tts(bot, event, apiToken, say_message, say_voice);
            if (res != 0)
            {
                bot.log(dpp::ll_debug, "[say_process] -> error encountered in call to tts.");
            }
        }
    }
}