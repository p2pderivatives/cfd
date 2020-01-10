FROM node

RUN apt-get update && apt-get install -y build-essential python
RUN git clone https://github.com/Kitware/CMake.git /tmp/cmake
WORKDIR /tmp/cmake
RUN git checkout tags/v3.14.2 \
    && ./bootstrap \
    && make \
    && make install

RUN mkdir /opt/cfd
WORKDIR /opt/cfd
COPY . .

# build
RUN npm install && npm run cmake_all

# test
RUN npm run ctest
