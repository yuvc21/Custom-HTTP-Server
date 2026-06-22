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

### Phase 3 — Improvements
- [ ] Keep-Alive connections
- [ ] Request timeout handling
- [ ] Proper logging
- [ ] HTTPS/TLS support

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