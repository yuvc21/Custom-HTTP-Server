#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <string>

namespace HttpParser {
    // extracting URL path from HTTP request
    std::string extractPath(const std::string& request);

    // generating response based on path
    std::string generateResponse(const std::string& path);

    // checking if the path starts with /ech/o
    bool isEchoPath(const std::string& path);

    // extracting the string after /echo/
    std::string extractEchoString(const std::string& path);
}

#endif