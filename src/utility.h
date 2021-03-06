//
// Created by Garegin Margaryan on 8/29/18.
//

#ifndef MICRO_CPP_UTILITY_H
#define MICRO_CPP_UTILITY_H

#include <string>
#include <boost/beast/core/string.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <csignal>

namespace micro_cpp {

    namespace http = boost::beast::http;

    static std::condition_variable interrupt_condition_;
    static std::mutex interrupt_mutex_;

    class utility {

    public: // signal handling

        static void set_hook() {
            std::signal(SIGINT, utility::handle_user_interrupt);
        }

        static void handle_user_interrupt(int sig) {
            if (sig == SIGINT) {
                interrupt_condition_.notify_one();
            }
        }

        static void wait_for_user_interrupt() {
            std::unique_lock<std::mutex> lock { interrupt_mutex_ };
            interrupt_condition_.wait(lock);
            lock.unlock();
        }

    public: // http message handling

        static boost::beast::string_view mime_type(boost::beast::string_view path);
        static std::string path_cat(boost::beast::string_view base, boost::beast::string_view path);
    };

    // This function produces an HTTP response for the given
    // request. The type of the response object depends on the
    // contents of the request, so the interface requires the
    // caller to pass a generic lambda for receiving the response.
    template<class Body, class Allocator, class Send>
    void handle_request(
            boost::beast::string_view doc_root,
            http::request<Body, http::basic_fields<Allocator>>&& req,
            Send&& send)
    {
        // Returns a bad request response
        auto const bad_request =
                [&req](boost::beast::string_view why)
                {
                    http::response<http::string_body> res{http::status::bad_request, req.version()};
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = why.to_string();
                    res.prepare_payload();
                    return res;
                };

        // Returns a not found response
        auto const not_found =
                [&req](boost::beast::string_view target)
                {
                    http::response<http::string_body> res{http::status::not_found, req.version()};
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = "The resource '" + target.to_string() + "' was not found.";
                    res.prepare_payload();
                    return res;
                };

        // Returns a server error response
        auto const server_error =
                [&req](boost::beast::string_view what)
                {
                    http::response<http::string_body> res{http::status::internal_server_error, req.version()};
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = "An error occurred: '" + what.to_string() + "'";
                    res.prepare_payload();
                    return res;
                };

        // Make sure we can handle the method
        if( req.method() != http::verb::get &&
            req.method() != http::verb::head)
            return send(bad_request("Unknown HTTP-method"));

        // Request path must be absolute and not contain "..".
        if( req.target().empty() ||
            req.target()[0] != '/' ||
            req.target().find("..") != boost::beast::string_view::npos)
            return send(bad_request("Illegal request-target"));

        // Build the path to the requested file
        std::string path = utility::path_cat(doc_root, req.target());
        if(req.target().back() == '/')
            path.append("index.html");

        // Attempt to open the file
        boost::beast::error_code ec;
        http::file_body::value_type body;
        body.open(path.c_str(), boost::beast::file_mode::scan, ec);

        // Handle the case where the file doesn't exist
        if(ec == boost::system::errc::no_such_file_or_directory)
            return send(not_found(req.target()));

        // Handle an unknown error
        if(ec)
            return send(server_error(ec.message()));

        // Cache the size since we need it after the move
        auto const size = body.size();

        // Respond to HEAD request
        if(req.method() == http::verb::head)
        {
            http::response<http::empty_body> res{http::status::ok, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, utility::mime_type(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return send(std::move(res));
        }

        // Respond to GET request
        http::response<http::file_body> res{
                std::piecewise_construct,
                std::make_tuple(std::move(body)),
                std::make_tuple(http::status::ok, req.version())};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, utility::mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }

}


#endif //MICRO_CPP_UTILITY_H
