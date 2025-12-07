# Use a base image with common dependencies pre-installed
FROM pstlab/coco-base:latest

# Install the necessary dependencies
RUN apt update && apt install -y libpqxx-dev
RUN apt install -y net-tools telnet

# Expose the COCO application port
EXPOSE 8080

# Set build arguments
ARG CLIENT_DIR=/gui

# Clone and build USPE
RUN git clone --recursive https://github.com/CTEMT/Urban-Sensing-Engine \
    && cd Urban-Sensing-Engine \
    && mkdir build && cd build \
    && cmake -DCLIENT_DIR=${CLIENT_DIR} -DCMAKE_BUILD_TYPE=Release .. \
    && make -j$(nproc)

# Build the GUI application
RUN npm --prefix /Urban-Sensing-Engine/gui install && npm --prefix /Urban-Sensing-Engine/gui run build

# Move the built COCO files to the /app directory
RUN mv /Urban-Sensing-Engine/build/uspe /uspe \
    && mkdir -p /gui && mv /Urban-Sensing-Engine/gui/dist /gui \
    && rm -rf /Urban-Sensing-Engine

# Start the USPE application
CMD ["/uspe"]