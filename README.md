A Discord bot made using C++ with the D++ library.
- _bruhshell (bs) dev (Ethan Christensen, etchris)_

![Jade](https://github.com/user-attachments/assets/1045f56f-ab7d-4f3f-ae78-5f66201c0242)

## Setup Instructions

### Prerequisites
- Docker
- Docker Compose
- [Ollama](https://ollama.com/)
- [Discord Application](https://discord.com/developers/applications)

### Running the Bot

1. **Clone the Repository:**

    ```bash
    git clone https://github.com/ethanlchristensen/Jade.git
    cd Jade
    ```

2. **Create a `.env` File:**

    Within the root of your project, create a `.env` file with the following content:

    ```
    DISCORD_BOT_TOKEN=<TOKEN>
    USERS_TO_ID={"<common-name>": "<@ID>"} # This will overwrite names with the actaul @'s when generating llm responses
    OLLAMA_ENDPOINT=localhost:11434 # OR if hosted on another machine x.x.x.x:11434
    ```
   
   Make sure to replace `<token>` and other placeholders with your actual configuration.

3. **Use Docker Compose to Build and Run:**

   ```bash
   docker-compose up --build -d
   ```
   This command will set up and run the bot using the Dockerfile and docker-compose.yaml provided in the project.

   To check the logs:
   ```bash
   docker logs jade   
   ```
