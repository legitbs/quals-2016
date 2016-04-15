FROM ubuntu:xenial

RUN apt-get update; apt-get upgrade -y
RUN apt-get install -y build-essential gdb libc-dbg

WORKDIR /app

CMD ["/bin/bash"]
