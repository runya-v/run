#include <fstream>
#include <sstream>
#include <string>

#include <boost/lexical_cast.hpp>

#include "Log.hpp"

#include "MimeTypes.hpp"
#include "Reply.hpp"
#include "Request.hpp"
#include "HtmlMaker.hpp"
#include "RequestHandler.hpp"


namespace http_server {

    bool RequestHandler::urlDecode(const std::string& in, std::string& out) {
        out.clear();
        out.reserve(in.size());

        for (std::size_t i = 0; i < in.size(); ++i) {
            if (in[i] == '%') {
                if (i + 3 <= in.size()) {
                    int value = 0;
                    std::istringstream is(in.substr(i + 1, 2));

                    if (is >> std::hex >> value) {
                        out += static_cast< char >(value);
                        i += 2;
                    }
                    else {
                        return false;
                    }
                }
                else {
                    return false;
                }
            }
            else if (in[i] == '+') {
                out += ' ';
            }
            else {
                out += in[i];
            }
        }
        return true;
    }


    RequestHandler::RequestHandler(const std::string& doc_root)
        : _doc_root(doc_root)
    {}


    void RequestHandler::handleRequest(const Request &req, Reply &rep) {
        // Decode url to path.
        std::string request_path;

        if (not urlDecode(req._uri, request_path)) {
            rep = Reply::stockReply(Reply::bad_request);
            return;
        }

        // Request path must be absolute and not contain "..".
        if (request_path.empty() or request_path[0] not_eq '/' or request_path.find("..") not_eq std::string::npos) {
            rep = Reply::stockReply(Reply::bad_request);
            return;
        }

        // If path ends in slash (i.e. is a directory) then add "index.html".
        if (request_path[request_path.size() - 1] == '/') {
            request_path += "index.html";
        }

        // Determine the file extension.
        std::size_t last_slash_pos = request_path.find_last_of("/");
        std::size_t last_dot_pos   = request_path.find_last_of(".");
        std::string extension;

        if (last_dot_pos not_eq std::string::npos and (last_dot_pos > last_slash_pos)) {
            extension = request_path.substr(last_dot_pos + 1);
        }

        // Make new html version by template
        if (extension == "html") {
            LOG(INFO) << "Request HTML: `" << _doc_root + request_path << "`.";
            HtmlMaker html_maker(_doc_root + request_path); 
        }

        // Open the file to send back.
        std::string full_path = _doc_root + request_path;
        std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);

        if (not is) {
            rep = Reply::stockReply(Reply::not_found);
            LOG(WARNING) << "Can`t open requested file `" << _doc_root + request_path << "`.";
        }
        else {
            // Fill out the Reply to be sent to the client.
            rep._status = Reply::ok;
            char buf[512];

            while (is.read(buf, sizeof(buf)).gcount() > 0) {
                rep._content.append(buf, is.gcount());
            }
            rep._headers.resize(2);
            rep._headers[0] = { "Content-Length", rep._headers[0]._value = boost::lexical_cast<std::string>(rep._content.size()) };
            rep._headers[1] = { "Content-Type", mime_types::ExtensionToType(extension) };
        }
    }
}
