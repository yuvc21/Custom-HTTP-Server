#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <string>

namespace HttpParser {
// Extracting URL path from HTTP request
// Example: "GET /echo/hello HTTP/1.1" → "/echo/hello"
std::string extractPath(const std::string &request);

// Extracting HTTP method from request
// Example: "POST /files/test HTTP/1.1" → "POST"
std::string extractMethod(const std::string &request);

// Extracting body from HTTP request (for POST)
// Body comes after "\r\n\r\n" separator
std::string extractBody(const std::string &request);

// Extracting Accept-Encoding header value
// Example: "Accept-Encoding: gzip, deflate" → "gzip, deflate"
std::string
extractAcceptEncoding(const std::string &request); //

// Compressing data with GZIP format (DEFLATE algorithm)
// Returns compressed binary data or empty string on error
std::string gzipCompress(const std::string &data);

// Version 1: Basic (2 parameters - GET only, no POST, no compression)
// Used by older code for backward compatibility
std::string generateResponse(const std::string &path,
                             const std::string &directory);

// Version 2: With POST support (4 parameters - GET + POST, no compression)
// Handles both GET and POST methods
std::string generateResponse(const std::string &method, const std::string &path,
                             const std::string &body,
                             const std::string &directory);

// Version 3: Full version (5 parameters - GET + POST + GZIP compression)
// Handles all methods and checks if client accepts gzip
std::string generateResponse(const std::string &method, const std::string &path,
                             const std::string &body,
                             const std::string &directory,
                             const std::string &acceptEncoding);

// ════════════════════════════════════════════════════════════
// PATH CHECKING FUNCTIONS
// ════════════════════════════════════════════════════════════

// Checking if path starts with "/echo/"
// Example: "/echo/hello" → true, "/files/test" → false
bool isEchoPath(const std::string &path);

// Extracting string after "/echo/"
// Example: "/echo/hello" → "hello"
std::string extractEchoString(const std::string &path);

// ════════════════════════════════════════════════════════════
// FILE HANDLING FUNCTIONS
// ════════════════════════════════════════════════════════════

// Checking if path starts with "/files/"
bool isFilePath(const std::string &path);

// Extracting filename from path
// Example: "/files/test.txt" → "test.txt"
std::string extractFileName(const std::string &path);

// Reading entire file contents into string
// Opens in binary mode, returns empty string on error
std::string readFile(const std::string &filePath);

// Checking if file exists on filesystem (uses stat system call)
bool fileExists(const std::string &filePath);

// Writing content to file (creates or overwrites)
// Returns true on success, false on error
bool writeFile(const std::string &filePath, const std::string &content);

} // namespace HttpParser

#endif // HTTP_PARSER_HPP