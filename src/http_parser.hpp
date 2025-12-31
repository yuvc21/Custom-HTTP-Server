#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <string>

namespace HttpParser {
    // extracting URL path from HTTP request
    std::string extractPath(const std::string& request);

    
    std::string extractMethod(const std::string& request);
    std::string extractBody(const std::string& request);

    
    // generating response based on path
    std::string generateResponse(const std::string& path, const std::string &directory);

    std::string generateResponse(const std::string& method, const std::string& path, const std::string &body, const std::string& directory);

    // checking if the path starts with /echo/
    bool isEchoPath(const std::string& path);

    // extracting the string after /echo/
    std::string extractEchoString(const std::string& path);

    // functions for file handling
    bool isFilePath(const std::string& path);
    std::string extractFileName(const std::string &path);
    std::string readFile(const std::string& filePath);
    bool fileExists(const std::string& filePath);

    bool writeFile(const std::string& filePath, const std::string& content);
}

#endif