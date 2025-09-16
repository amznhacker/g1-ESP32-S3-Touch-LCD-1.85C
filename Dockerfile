FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    git wget flex bison gperf python3 python3-pip python3-venv \
    cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 \
    && rm -rf /var/lib/apt/lists/*

# Install ESP-IDF
RUN git clone --recursive https://github.com/espressif/esp-idf.git /opt/esp-idf
WORKDIR /opt/esp-idf
RUN git checkout v5.3.1 && git submodule update --init --recursive
RUN ./install.sh esp32s3

# Set environment
ENV IDF_PATH=/opt/esp-idf
ENV PATH="$IDF_PATH/tools:$PATH"

WORKDIR /workspace