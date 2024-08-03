FROM ubuntu:latest

COPY . /trading-bot-cpp

ENV LD_LIBRARY_PATH=/usr/local/lib

RUN cd /trading-bot-cpp && bash prepare.sh
RUN cd /trading-bot-cpp && mkdir -p build && cd build && cmake .. && make
RUN cd /trading-bot-cpp/build && ctest

CMD [ "trading-bot-cpp/build/app/trading_bot_app" ]
