# Custom HTTP Server

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![License](https://img.shields.io/badge/license-MIT-green?style=for-the-badge)

An HTTP/1.1 server built from scratch in C++ as part of the [CodeCrafters](https://codecrafters.io) challenge.

</div>

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [What is HTTP/1.1](#what-is-http11)
- [What are POSIX Sockets](#what-are-posix-sockets)
- [How GZIP Compression Works](#how-gzip-compression-works)
- [How Threading Works Here](#how-threading-works-here)
- [Tech Stack](#tech-stack)
- [Project Structure](#project-structure)
- [Installation](#installation)
- [Usage](#usage)
- [API Endpoints](#api-endpoints)
- [Architecture](#architecture)
- [Testing](#testing)
- [Roadmap](#roadmap)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

## Overview

A lightweight HTTP/1.1 server written in modern C++ using raw POSIX sockets — no external HTTP libraries. Handles TCP connections, parses HTTP requests, generates compliant responses, serves static files, and supports gzip compression.

Built as part of the CodeCrafters "Build Your Own HTTP Server" challenge.

---

## Features

- **TCP Socket Server** — binds to port 4221, accepts incoming connections
- **HTTP Request Parsing** — extracts method, path, headers, and body from raw requests
- **Dynamic Routing** — path-based request handling
- **Status Codes** — `200 OK`, `201 Created`, `404 Not Found`, `500 Internal Server Error`
- **Echo Endpoint** — `/echo/{str}` echoes back the provided string
- **Content Headers** — correct `Content-Type` and `Content-Length` on all responses
- **Concurrent Connections** — one thread per client via `std::thread`
- **File Serving** — GET `/files/{name}` serves files from a configurable directory
- **File Upload** — POST `/files/{name}` writes request body to disk
- **GZIP Compression** — compresses responses when client sends `Accept-Encoding: gzip`

---

## What is HTTP/1.1

HTTP (HyperText Transfer Protocol) is the foundation of data communication on the web. Version 1.1, defined in [RFC 9112](https://datatracker.ietf.org/doc/html/rfc9112), is the most widely deployed version and introduced several improvements over HTTP/1.0.

An HTTP request looks like this:

```
GET /echo/hello HTTP/1.1
Host: localhost:4221
Accept-Encoding: gzip
User-Agent: curl/7.81.0

```

And a response looks like this:

```
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 5

hello
```

Every HTTP message has three parts:
- **Request/Status line** — the method, path, and protocol version (or status code for responses)
- **Headers** — key-value metadata like `Content-Type`, `Content-Length`, `Accept-Encoding`
- **Body** — the actual payload, separated from headers by `\r\n\r\n`

This server manually parses each of these from the raw byte stream received over the socket, with no help from any HTTP library.

Key HTTP/1.1 improvements over 1.0:
- Persistent connections (Keep-Alive by default)
- Chunked transfer encoding
- The `Host` header is mandatory
- Better caching controls

---

## What are POSIX Sockets

POSIX sockets are a standard API for network communication on Unix-based systems (Linux, macOS). They allow programs to send and receive data over a network using the same `read()`/`write()` model as files.

The lifecycle of a TCP server socket looks like this:

```
socket()   → create a socket file descriptor
bind()     → attach it to an IP address and port
listen()   → mark it as passive (ready to accept)
accept()   → block until a client connects, return a new fd
recv()     → read data from the client
send()     → write data back to the client
close()    → close the connection
```

In this server, all of this is done manually using system calls from `<sys/socket.h>` and `<unistd.h>`. There is no Boost.Asio, no libuv, no abstraction layer — just direct calls into the Linux kernel.

When you call `send(client_fd, response.c_str(), response.length(), 0)`, your program is asking the Linux kernel to take that buffer and push it out through the network interface. The kernel handles the actual TCP segmentation, checksums, retransmission, and delivery.

---

## How GZIP Compression Works

GZIP is a file format and compression algorithm based on DEFLATE (a combination of LZ77 and Huffman coding). It is the most common HTTP compression format.

When a client (like curl or a browser) supports compression, it sends:

```
Accept-Encoding: gzip
```

The server sees this header, compresses the response body using zlib, and responds with:

```
Content-Encoding: gzip
Content-Length: <compressed size>
```

The client then decompresses the body transparently before handing it to the application.

In this server, compression is done using zlib's `deflate` API with `windowBits = 15 + 16` (the `+16` enables gzip wrapping instead of raw DEFLATE):

```cpp
deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
```

GZIP is particularly effective for text-based responses (HTML, JSON, plain text) where repeated patterns compress well. Binary files like images or already-compressed formats gain little to nothing from it.

---

## How Threading Works Here

By default, `accept()` is a blocking call — it halts the program until a client connects. If you handle each client in the same thread, the server can only serve one client at a time. While handling client A, client B waits.

This server solves that by spawning a new `std::thread` for every incoming connection:

```cpp
std::thread clientThread([this, client_fd] { handleClient(client_fd); });
clientThread.detach();
```

`detach()` means the thread runs independently — the main thread immediately loops back to `accept()` and is ready for the next client without waiting for the previous one to finish.

Each thread gets its own stack and handles exactly one client: read request → parse → generate response → send → close. Once done, the thread exits and its resources are cleaned up automatically.

This is a simple one-thread-per-connection model. It works well for moderate loads (100+ concurrent connections). For extremely high concurrency, more advanced patterns like thread pools or async I/O (epoll) would be used instead.

---

## Tech Stack

| Component | Technology |
|-----------|------------|
| Language | C++17 |
| Build System | CMake 3.20+ |
| Network API | POSIX Sockets (`sys/socket.h`) |
| Compression | zlib |
| Protocol | HTTP/1.1 (RFC 9112) |
| Platform | Linux (Arch, Ubuntu, Debian) |

---

## Project Structure

```
Custom-HTTP-Server/
├── src/
│   ├── main.cpp           # entry point, CLI argument parsing
│   ├── server.cpp         # TCP socket, accept loop, thread spawning
│   ├── server.hpp
│   ├── http_parser.cpp    # request parsing, response generation, gzip
│   └── http_parser.hpp
├── CMakeLists.txt
├── run.sh                 # build + run in one shot
└── README.md
```

---

## Installation

### Prerequisites

**Arch Linux**
```bash
sudo pacman -S cmake gcc zlib
```

**Ubuntu/Debian**
```bash
sudo apt install cmake g++ zlib1g-dev
```

### Build and Run

```bash
git clone https://github.com/yuvc21/Custom-HTTP-Server.git
cd Custom-HTTP-Server

chmod +x run.sh
./run.sh --directory /tmp
```

`run.sh` builds and starts the server in one shot. To rebuild from scratch:

```bash
rm -rf build/
./run.sh --directory /tmp
```

---

## Usage

```bash
# start server (serves files from /tmp)
./run.sh --directory /tmp

# or run the binary directly after building
./build/http-server --directory /tmp
```

Server listens on `0.0.0.0:4221`.

---

## API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| `GET` | `/` | Health check — returns `200 OK` |
| `GET` | `/echo/{str}` | Returns `str` as plain text |
| `GET` | `/files/{filename}` | Serves file from the configured directory |
| `POST` | `/files/{filename}` | Writes request body to file |

---

## Architecture

### Request Flow

```
Client
  │
  ▼
TCP Accept (port 4221)
  │
  ▼
std::thread → handleClient()
  │
  ├── HttpParser::extractMethod()
  ├── HttpParser::extractPath()
  ├── HttpParser::extractBody()
  ├── HttpParser::extractAcceptEncoding()
  │
  ▼
HttpParser::generateResponse()
  │
  ├── POST /files/  → write to disk → 201
  ├── GET  /files/  → read from disk → 200
  ├── GET  /echo/   → echo string → 200
  ├── GET  /        → 200
  └── anything else → 404
  │
  ▼
send() → close()
```

---

## Testing

Start the server in one terminal:

```bash
./run.sh --directory /tmp
```

Then in another terminal:

```bash
# root
curl -i http://localhost:4221/

# echo
curl -i http://localhost:4221/echo/hello

# 404
curl -i http://localhost:4221/nonexistent

# upload a file, then fetch it
curl -i -X POST http://localhost:4221/files/test.txt -d "hello server"
curl -i http://localhost:4221/files/test.txt

# gzip compression
curl -i --compressed -H "Accept-Encoding: gzip" http://localhost:4221/echo/hello

# concurrent stress test (20 requests)
for i in {1..20}; do curl -s http://localhost:4221/echo/test & done; wait && echo "all done"
```

---

## Roadmap

### Phase 1 — Core
- [x] TCP socket server
- [x] HTTP request parsing
- [x] Response generation (200, 404)
- [x] Echo endpoint

### Phase 2 — Features
- [x] Multi-threaded connection handling
- [x] Static file serving (GET)
- [x] File upload (POST)
- [x] GZIP compression


---

## License

MIT — see [LICENSE](LICENSE).

---

## Acknowledgments

- [CodeCrafters](https://codecrafters.io) — for the challenge
- [RFC 9112](https://datatracker.ietf.org/doc/html/rfc9112) — HTTP/1.1 spec
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — socket programming reference

---

<div align="center">

Made with ❤️ and C++

</div>