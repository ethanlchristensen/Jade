#include "apis.h"
#include "helpers.cpp"

std::string llm(const std::string& apiToken, const std::string& system_prompt, const std::string& user_prompt) {
    CURL *curl;
    CURLcode response;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = nullptr;
        std::string auth = fmt::format("Authorization: Bearer {}", apiToken);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth.c_str());
        std::string data = fmt::format(
            R"({{"model": "{}","messages": [{{"role": "system","content": "{}" }},{{"role": "user", "content": "{}" }}],"temperature": 1,"max_tokens": 500, "top_p": 1,"frequency_penalty": 0,"presence_penalty": 0 }})",
            MODEL, system_prompt, user_prompt
        );

        curl_easy_setopt(curl, CURLOPT_URL, OPENAIURL);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        response = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        // did we get response
        if(response == 0) {
            json responseJson = json::parse(readBuffer);
            return responseJson["choices"][0]["message"]["content"];
        } else {
            return "Hey, I am not in the mood to talk right now.";
        }
    }
    return "Hey, I am not in the mood to talk right now.";
}

std::int8_t tts(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken, const std::string& text, const std::string& voice) {
    FILE *fp;
    CURL *curl;
    CURLcode response;
    errno_t err;
    std::vector<uint8_t> pcmdata;
    mpg123_init();

    int error = 0;
    unsigned char* buffer;
    size_t buffer_size, done;
    int channels, encoding;
    long rate;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {

        bot.log(dpp::ll_debug, "[tts] -> attempting to download tts message.");

        err = fopen_s(&fp, TTSFILE, "wb");

        if (err != 0) {
            return -1;
        }

        struct curl_slist *headers = nullptr;
        std::string auth = fmt::format("Authorization: Bearer {}", apiToken);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth.c_str());
        std::string data = fmt::format(
                R"({{"model": "{}","input": "{}", "voice":"{}"}})",
                TTSMODEL, text, voice
        );

        std::cout << data << std::endl;

        curl_easy_setopt(curl, CURLOPT_URL, OPENAITTS);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);

        response = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        fclose(fp);
    }

    bot.log(dpp::ll_debug, "[tts] -> attempting to read tts.mp3 to send to vc.");
    mpg123_handle *mh = mpg123_new(nullptr, &error);
    mpg123_param(mh, MPG123_FORCE_RATE, 94000, 94000.0);

    buffer_size = mpg123_outblock(mh);
    buffer = new unsigned char[buffer_size];

    mpg123_open(mh, TTSFILE);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    unsigned int counter = 0;
    for (size_t totalBytes = 0; mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK; ) {
        for (size_t i = 0; i < buffer_size; i++) {
            pcmdata.push_back(buffer[i]);
        }
        counter += buffer_size;
        totalBytes += done;
    }

    delete[] buffer;
    mpg123_close(mh);
    mpg123_delete(mh);

    dpp::voiceconn* channel = event.from->get_voice(event.command.guild_id);

    if (channel && channel->voiceclient && channel->voiceclient->is_ready()) {
        bot.log(dpp::ll_debug, "[tts] -> sending tts audio to the vc!");
        channel->voiceclient->send_audio_raw((uint16_t *) pcmdata.data(), pcmdata.size());
    } else {
        dpp::message msg(event.command.channel_id, "Hey, I would love to talk, but I am not in a VC right now.");
        bot.message_create(msg);
    }

    return 0;
}
