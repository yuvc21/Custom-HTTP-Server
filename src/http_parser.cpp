#include "http_parser.hpp"

namespace HttpParser{
    std::string extractPath(const std::string& request){
        // find the first space (after HTTP method like "GET");
        size_t first_space = request.find(' ');
        if(first_space == std::string::npos){
            return "";
        }
        // find second space (after the path, before "HTTP/1.1");
        size_t second_space = request.find(' ', first_space + 1);
        if(second_space == std::string::npos){
            return "";
        }
        return request.substr(first_space + 1, second_space - first_space - 1);
    }

    bool isEchoPath(const std::string& path){
        //checking if the path starts with "/echo/"
        return path.find("/echo/") == 0;
    }
    std::string extractEchoString(const std::string& path){
        // extracting everything after "/echo/"
        if(isEchoPath(path)){
            return path.substr(6); // as "/echo/" is 6 characters
        }
        return "";
    }
    std::string generateResponse(const std::string& path){
        if(isEchoPath(path)){
            std::string echo_str = extractEchoString(path);
            int content_length = echo_str.length();

            // build response with headers and body
            std::string response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: text/plain\r\n";
            response += "Content-Length: " + std::to_string(content_length) + "\r\n";
            response += "\r\n"; //end of headers
            response += echo_str; //response body

            return response;
        }
        if(path == "/"){
            return "HTTP/1.1 200 OK\r\n\r\n";
        }else{
            return "HTTP/1.1 404 Not Found\r\n\r\n";
        }
    }
}