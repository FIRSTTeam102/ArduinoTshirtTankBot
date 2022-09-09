# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.245.2/containers/ubuntu/.devcontainer/base.Dockerfile

# [Choice] Ubuntu version (use ubuntu-22.04 or ubuntu-18.04 on local arm64/Apple Silicon): ubuntu-22.04, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="jammy"
FROM mcr.microsoft.com/vscode/devcontainers/base:0-${VARIANT}

# ARG ARDUINO_CLI_DIR='/var/lib/docker/codespacemount/workspace/arduino-cli'
ARG ARDUINO_CLI_DIR='/home/vscode/.local/bin'
# this can't be accessed in docker?
ARG ARDUINO_DATA_DIR='/workspaces/arduino'

RUN mkdir -p $ARDUINO_DATA_DIR
RUN chown vscode $ARDUINO_DATA_DIR

# Become a non-root user
WORKDIR /home/vscode/
USER vscode

RUN mkdir -p $ARDUINO_CLI_DIR
ENV PATH="$ARDUINO_CLI_DIR:${PATH}"
# extension only supports an older cli version
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=$ARDUINO_CLI_DIR sh -s 0.13.0
# RUN $ARDUINO_CLI_DIR/arduino-cli config init --dest-dir $ARDUINO_DATA_DIR
# ENV ARDUINO_DIRECTORIES_DATA=$ARDUINO_DATA_DIR/.arduino
# ENV ARDUINO_DIRECTORIES_DOWNLOADS=$ARDUINO_DATA_DIR/.arduino/staging
# ENV ARDUINO_DIRECTORIES_USER=$ARDUINO_DATA_DIR
# done after docker setup because this won't work idk
# RUN $ARDUINO_CLI_DIR/arduino-cli core install arduino:avr
