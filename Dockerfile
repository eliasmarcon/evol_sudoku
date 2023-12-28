FROM ubuntu:latest

ARG USERNAME=jlangela

RUN useradd -m -s /bin/bash ${USERNAME} && \
    mkdir -p /etc/sudoers.d && \
    echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" | tee /etc/sudoers.d/${USERNAME}

RUN apt-get update && apt-get install -y wget gcc git make g++

# Set non-interactive frontend (useful for Docker builds)
ENV DEBIAN_FRONTEND noninteractive

WORKDIR /tmp

RUN wget http://lancet.mit.edu/ga/dist/galib247.tgz && tar xzvf galib247.tgz

WORKDIR /tmp/galib247

RUN sed -i '/^CXXFLAGS\s*=/ s/$/ -O2 -fpermissive/' makevars && make && make install

# Switch to the new user
USER ${USERNAME}

# Set the default command to bash (optional)
CMD ["/bin/bash"]