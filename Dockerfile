FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    git wget flex bison gperf python3 python3-pip python3-venv \
    cmake ninja-build ccache libffi-dev libssl-dev dfu-util \
    libusb-1.0-0 udev && \
    rm -rf /var/lib/apt/lists/*

# Install ESP-IDF
RUN git clone --recursive --depth 1 --branch v5.3.1 https://github.com/espressif/esp-idf.git /opt/esp-idf
RUN cd /opt/esp-idf && ./install.sh esp32s3

# Set environment
ENV IDF_PATH=/opt/esp-idf
ENV PATH="$IDF_PATH/tools:$PATH"

WORKDIR /workspace
CMD ["/bin/bash"]