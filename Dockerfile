# C:\Users\ethan\Desktop\Github\Jade\Dockerfile
FROM debian:stable-slim

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    libssl-dev \
    gcc \
    g++ \
    zlib1g-dev \
    libsodium-dev \
    libopus-dev \
    ffmpeg \
    libspdlog-dev \
    opus-tools \
    libfmt-dev \
    libavformat-dev \
    libavcodec-dev \
    libavutil-dev \
    libavfilter-dev \
    libcurl4-openssl-dev \
    libmpg123-dev \
    mpg123 \
    libcodec2-dev \
    nlohmann-json3-dev \
    python3 \
    python3-pip && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Install yt-dlp via pip
RUN python3 -m pip install --break-system-packages --no-cache-dir yt-dlp

# Clone, build, and install cppcodec
RUN git clone https://github.com/tplgy/cppcodec.git && \
    cd cppcodec && \
    mkdir build && cd build && \
    cmake .. && \
    make && make install && \
    cd .. && rm -rf cppcodec

# Download and install DPP
RUN wget -O dpp.deb https://dl.dpp.dev/latest/linux-rpi-arm64 && \
    dpkg -i dpp.deb && \
    rm dpp.deb

# Set working directory
WORKDIR /usr/src/app

# Copy source and configuration files
COPY CMakeLists.txt .
COPY ./src /usr/src/app/src
COPY ./include /usr/src/app/include

# Build the application using a mounted volume
RUN mkdir -p /app/build && \
    cmake -B /app/build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build /app/build

# Move the final executable to /app
RUN cp /app/build/Jade /app/

# Set the working directory for the entrypoint
WORKDIR /app

# Set the entry point for the container
ENTRYPOINT ["./Jade"]
