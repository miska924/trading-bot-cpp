# Укажите базовый образ, который будет использоваться для контейнера
FROM ubuntu:latest

# Копируйте скрипт prepare.sh в контейнер
COPY . /trading-bot-cpp

# Выполните скрипт при сборке контейнера
RUN apt-get update
RUN apt-get install -y git libjsoncpp-dev g++ ninja-build libyaml-cpp-dev gnuplot wget unzip uuid-dev zlib1g-dev python3-pip libssl-dev
RUN pip3 install cmake
RUN cd /trading-bot-cpp && bash prepare.sh
RUN cd /trading-bot-cpp && mkdir -p build && cd build && cmake .. && make
RUN ctest
