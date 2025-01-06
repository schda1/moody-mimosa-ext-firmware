ARG VARIANT=ubuntu-22.04
FROM mcr.microsoft.com/vscode/devcontainers/base:${VARIANT}

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update

# Install dependencies for GUI (X11, Qt5, etc.)
RUN apt-get update && \
    apt-get install -y \
    libglib2.0-0 \
    libxcb1 \
    libgl1-mesa-glx \
    x11-apps \
    libx11-dev \
    libxrender-dev \
    autoconf \
    cmake \
    clang-format \
    && apt-get clean

# Install arm gcc compiler for stm32 logger application 
RUN apt-get install -y gcc-arm-none-eabi

# Allow access to X server (from within the container)
RUN apt-get install -y x11-xserver-utils

# Set environment variable for DISPLAY to point to Windows host (Docker for Windows uses host.docker.internal)
ENV DISPLAY=host.docker.internal:0
