# Build stage
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    g++ cmake libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir build && cd build && cmake .. && make -j4

# Runtime stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libcurl4 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy binary
COPY --from=builder /app/build/flight_service .

# Copy static UI
COPY --from=builder /app/static ./static

EXPOSE 8080

CMD ["./flight_service", "8080"]