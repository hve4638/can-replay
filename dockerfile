FROM ubuntu:latest

RUN echo "export PS1='\[\e]0;\u@\h: \w\a\]${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '" >> /root/.bashrc

RUN set -x \
&& apt update

RUN apt install python3 -y \
&& apt install tshark -y \
&& apt-get clean

RUN addgroup --gid 1000 user \
&& adduser --home /home/user --gid 1000 --shell /bin/bash --uid 1000 --gecos '' --disabled-password user \
&& echo 'user:1324' | chpasswd \
&& echo 'umask 000' >> /home/user/.bashrc \
&& echo 'export umask' >> /home/user/.bashrc \
&& echo "export PS1='\[\e]0;\u@\h: \w\a\]${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '" >> /home/user/.bashrc

RUN mkdir -p /code

COPY ./build/canreplay /code
COPY ./binarization /code/binarization
COPY ./pcapparser /code/pcapparser
COPY ./analyzer /code/analyzer

ENV EXPORT_PCAPPARSER=/vol
ENV EXPORT_BINARIZAION=/vol
ENV EXPORT_ANALYZER=/vol

USER user

WORKDIR "/vol"
