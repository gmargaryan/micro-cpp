//
// Created by Garegin Margaryan on 8/24/18.
//

#ifndef MICRO_CPP_BASE_SESSION_H
#define MICRO_CPP_BASE_SESSION_H

#include <memory>
#include <boost/beast/http/message.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core/flat_buffer.hpp>


namespace micro_cpp {

    class session : public std::enable_shared_from_this<session> {

    public:
        explicit session(boost::asio::ip::tcp::socket sc, std::shared_ptr<std::string const> const& doc_root);
        void run();
        void do_read();
        void on_read(boost::system::error_code ec, std::size_t bytes_transferred);
        void on_write(boost::system::error_code ec, std::size_t bytes_transferred, bool close);
        void do_close();

    private:
        std::shared_ptr<void> res_;
        boost::asio::ip::tcp::socket socket_;
        boost::beast::flat_buffer buffer_;
        boost::asio::strand<boost::asio::io_context::executor_type> strand_;
        std::shared_ptr<std::string const> doc_root_;
        boost::beast::http::request<boost::beast::http::string_body> req_;


        struct send_http {

            session& self_;

            explicit send_http(session& s): self_{s} {}

            template <bool isRequest, class Body, class Field>
                    void operator()(boost::beast::http::message<isRequest, Body, Field>&& msg) const {

                auto sp = std::make_shared<boost::beast::http::message<isRequest, Body, Field>>(std::move(msg));

                self_.res_ = sp;

                // Write the response
                boost::beast::http::async_write(self_.socket_, *sp, boost::asio::bind_executor(self_.strand_, std::bind(&session::on_write, self_.shared_from_this(), std::placeholders::_1, std::placeholders::_2, sp->need_eof())));
            }
        } send_http_;// Functional object send_http
    };
}


#endif //MICRO_CPP_BASE_SESSION_H
