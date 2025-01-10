FROM debian:stable-slim

WORKDIR /usr/src/app

COPY ./src /usr/src/app/src
COPY ./include /usr/src/app/include
#COPY ./cmake /usr/src/app/cmake
COPY CMakeLists.txt .
COPY Dockerfile .

WORKDIR /usr/src/app/build

# COPY .env .
RUN apt-get update && apt-get install -y wget libssl-dev gcc g++ cmake zlib1g-dev libsodium-dev libopus-dev ffmpeg libspdlog-dev opus-tools libopus-dev libspdlog-dev libfmt-dev libssl-dev libavformat-dev libavcodec-dev libavutil-dev libavfilter-dev libcurl4-openssl-dev yt-dlp libmpg123-dev mpg123
RUN wget -O dpp.deb https://dl.dpp.dev/latest/linux-rpi-arm64
RUN dpkg -i dpp.deb

RUN cmake ..
RUN make

# Set the entry point for the container
ENTRYPOINT ["./Jade"]