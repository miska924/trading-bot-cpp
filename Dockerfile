FROM ubuntu:latest

COPY . /trading-bot-cpp

RUN cd /trading-bot-cpp && bash prepare.sh
RUN cd /trading-bot-cpp && mkdir -p build && cd build && cmake .. && make
RUN cd /trading-bot-cpp/build && ctest
