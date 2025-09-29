


# Base we build upon
FROM ubuntu:22.04


# Download Linux support tools
#unzip, libgl1 and libglib2.0-0 are for commander support
RUN apt-get update && \
    apt-get clean && \
    apt-get install -y \
    build-essential \
    wget \
    curl \
    unzip \
    libgl1 \
    libglib2.0-0 \
    libncurses5

# Set up a development tools directory
WORKDIR /home/dev

# Get and install Arm none eabi GCC compiler
RUN wget -qO- https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 | tar -xj

# Install Simplicity Commander (unfortunately no stable URL available, this
# is known to be working with Commander_linux_x86_64_1v15p0b1306.tar.bz).
# RUN \
#     curl -O https://www.silabs.com/documents/login/software/SimplicityCommander-Linux.zip \
#     && unzip SimplicityCommander-Linux.zip \
#     && tar -C /opt -xjf SimplicityCommander-Linux/Commander-cli_linux_x86_64_*.tar.bz \
#     && rm -r SimplicityCommander-Linux \
#     && rm SimplicityCommander-Linux.zip

# The above method doesn't work anymore because the server seems to require authentication/cookies.
# As a workaround, manually download the file "SimplicityCommander-Linux.zip" from
# https://www.silabs.com/documents/login/software/SimplicityCommander-Linux.zip
# and place it in the same directory as this Dockerfile,

COPY SimplicityCommander-Linux.zip .
RUN unzip SimplicityCommander-Linux.zip \
    && tar -C /opt -xjf SimplicityCommander-Linux/Commander_linux_x86_64_*.tar.bz \
    && rm -r SimplicityCommander-Linux \
    && rm SimplicityCommander-Linux.zip

ENV PATH="$PATH:/opt/commander"

# Get and install JLink tools (for flashing/debugging)
RUN curl -d "accept_license_agreement=accepted&submit=Download%20software" https://www.segger.com/downloads/jlink/JLink_Linux_V830_x86_64.tgz --output ./jflash.tgz \
&& tar -xaf jflash.tgz
# && rm jflash.tgz

# set environment path
ENV PATH=$PATH:/home/dev/gcc-arm-none-eabi-10.3-2021.10/bin

ENV PATH=$PATH:$PATH:/home/dev/JLink_Linux_V830_x86_64

# set working directory for our app
WORKDIR /home/app
ADD . /home/app

#to build image use:
# docker image build -t erksponge/gcc_arm_commander_jflash .