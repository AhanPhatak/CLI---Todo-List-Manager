# Multi-stage Dockerfile for C++ Todo CLI

# Stage 1: Builder
FROM ubuntu:22.04 AS builder

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libmysqlclient-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Create build directory and build
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    cmake --build .

# Stage 2: Runtime
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libmysqlclient21 \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy compiled binary from builder
COPY --from=builder /app/build/todo /app/todo

# Make binary executable
RUN chmod +x /app/todo

# Set environment variables for MySQL connection
ENV TODO_DB_HOST=mysql
ENV TODO_DB_USER=todo_user
ENV TODO_DB_PASS=todo_password
ENV TODO_DB_NAME=todo_cli_db
ENV TODO_DB_PORT=3306

# Entry point - run the todo CLI
ENTRYPOINT ["/app/todo"]

# Default command (show usage)
CMD [""]
