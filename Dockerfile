FROM archlinux

RUN pacman -Syu --noconfirm openssl zlib sqlite curl jansson make gcc cmake

WORKDIR /usr/bazcal

COPY . .

WORKDIR /usr/bazcal/bin

RUN cmake ..

RUN make

WORKDIR /data

ENTRYPOINT [ "/usr/bazcal/bin/bazcal-lf" ]
