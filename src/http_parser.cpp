#include "http_parser.hpp"
#include "server.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
/*
// <fstream> = Library for opening files
std::ifstream file("test.txt");  // Like opening a book 📖

// <sstream> = Library for temporary text storage
std::stringstream buffer;        // Like a notepad 📝
buffer << file.rdbuf();          // Copy book content to notepad

// <sys/stat.h> = Library for checking file info
stat("test.txt", &info);         // Like asking "Does this book exist?" 🔍
  struct stat buffer;                    // ← Needs <sys/stat.h>
  return (stat(filePath.c_str(), &buffer) == 0);  // ← Needs <sys/stat.h>
*/

namespace HttpParser {
std::string extractPath(const std::string &request) {
  // find the first space (after HTTP method like "GET");
  size_t first_space = request.find(' ');
  if (first_space == std::string::npos) {
    return "";
  }
  // find second space (after the path, before "HTTP/1.1");
  size_t second_space = request.find(' ', first_space + 1);
  if (second_space == std::string::npos) {
    return "";
  }
  return request.substr(first_space + 1, second_space - first_space - 1);
}

bool isEchoPath(const std::string &path) {
  // checking if the path starts with "/echo/"
  return path.find("/echo/") == 0;
}
std::string extractEchoString(const std::string &path) {
  // extracting everything after "/echo/"
  if (isEchoPath(path)) {
    return path.substr(6); // as "/echo/" is 6 characters
  }
  return "";
}

// checking if path is /files/
bool isFilePath(const std::string &path) { return path.find("/files/") == 0; }

// extracting filename from /files/filename
std::string extractFileName(const std::string &path) {
  if (isFilePath(path)) {
    return path.substr(7); // "/files/" is 7 characters
  }
  return "";
}
// checking if the file exists
bool fileExists(const std::string &filePath) {
  struct stat buffer;
  return (stat(filePath.c_str(), &buffer) == 0);
}

// reading file contents
std::string readFile(const std::string &filePath) {
  std::ifstream file(filePath, std::ios::binary);
  if (!file.is_open()) {
    return "";
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

std::string generateResponse(const std::string &method, const std::string &path, const std::string &body, const std::string &directory){

  // handling POST /files/filename
  if(method == "POST" && isFilePath(path)){
    std::string filename = extractFileName(path);
    std::string filepath = directory + "/" + filename;

    // writing the body content to file
    if(writeFile(filepath, body)){
      return "HTTP/1.1 201 Created\r\n\r\n"; //success
    }else{
      return "HTTP/1.1 500 internal Server Error\r\n\r\n";
    }
  }else{
    // for the GET requests
    return generateResponse(path , directory);
  }
}
std::string generateResponse(const std::string &path,
                             const std::string &directory) {
  // handling the /files/ endpoint
  if (isFilePath(path)) {
    std::string filename = extractFileName(path);
    std::string filepath = directory + "/" + filename;

    // checking if the file exists
    if (fileExists(filepath)) {
      // reading the file content
      std::string content = readFile(filepath);
      content += "\n";
      size_t content_length = content.length();

      // building the response
      std::string response = "HTTP/1.1 200 OK\r\n";
      response += "Content-Type: application/octet-stream\r\n";
      response += "Content-Length: " + std::to_string(content_length) + "\r\n";
      response += "\r\n";
      response += content;
      return response;
    } else {
      return "HTTP/1.1 404 Not Found\r\n\r\n";
    }
  }
  // handling the /echo/ endpoint
  if (isEchoPath(path)) {
    std::string echo_str = extractEchoString(path);
    int content_length = echo_str.length();

    // build response with headers and body
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Content-Length: " + std::to_string(content_length) + "\r\n";
    response += "\r\n";   // end of headers
    response += echo_str; // response body

    return response;
  }
  if (path == "/") {
    return "HTTP/1.1 200 OK\r\n\r\n";
  } else {
    return "HTTP/1.1 404 Not Found\r\n\r\n";
  }
}

std::string extractMethod(const std::string &request) {
  size_t first_space = request.find(' ');
  if (first_space == std::string::npos) {
    return "";
  }
  return request.substr(0, first_space);
}

std::string extractBody(const std::string &request) {
  size_t body_start = request.find("\r\n\r\n");
  if (body_start == std::string::npos) {
    return "";
  }
  return request.substr(body_start + 4); // +4 is for skipping the \r\n\r\n
}

bool writeFile(const std::string &filePath, const std::string &content) {
  std::ofstream file(filePath, std::ios::binary);
  if (!file.is_open()) {
    return false;
  }
  file << content << std::endl;
  file.close();
  return true;
}

} // namespace HttpParser