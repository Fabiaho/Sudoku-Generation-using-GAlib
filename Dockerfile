FROM ubuntu:latest

# Install C++ compiler, make, wget, unzip, and any other dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    wget \
    unzip \
    libmysqlcppconn-dev # MySQL Connector for C++

# Download and install GAlib
WORKDIR /tmp
RUN wget http://lancet.mit.edu/ga/dist/galib247.zip -O galib.zip \
    && unzip galib.zip \
    && mv galib247 ga \
    && cd ga \
    && sed -i '/^CXXFLAGS/s/$/ -fpermissive/' makevars \
    && make \
    && make install

# Copy the source code
COPY . /sudoku-generator

# Set the working directory
WORKDIR /sudoku-generator

# Compile the application using the Makefile
RUN make

# Set the entrypoint script
ENTRYPOINT ["./scripts/entrypoint.sh"]
