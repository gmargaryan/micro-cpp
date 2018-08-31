//
// Created by Garegin Margaryan on 8/24/18.
//

#include <boost/asio/bind_executor.hpp>
#include <boost/beast/http/error.hpp>
#include <iostream>
#include "session.h"
#include "utility.h"

namespace micro_cpp {

    //
    session::session(boost::asio::ip::tcp::socket sc, std::shared_ptr<std::string const> const& dr)
    : socket_(std::move(sc))
    , strand_(socket_.get_executor())
    , doc_root_(dr)
    , send_http_(*this){

    }

    //
    void session::run(){
        do_read();
    }

    //
    void session::do_read(){

        req_ = {};

        // Read the request
        boost::beast::http::async_read(socket_, buffer_, req_,
                         boost::asio::bind_executor(
                                 strand_,
                                 std::bind(&session::on_read,
                                         shared_from_this(),
                                         std::placeholders::_1,
                                         std::placeholders::_2)));
    }

    //
    void session::on_read(boost::system::error_code ec, std::size_t bytes_transferred){

        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if(ec == boost::beast::http::error::end_of_stream)
            return do_close();

        if(ec) {
            // Failed session::on_read
            std::cerr << "Failed session::on_read! Error Code = " << ec.value()
                      << ". Message: " << ec.message()
                      << '\n';
            return;
        }

        // Send the response
        handle_request(*doc_root_, std::move(req_), send_http_);
    }

    //
    void session::on_write(boost::system::error_code ec, std::size_t bytes_transferred, bool close){

        boost::ignore_unused(bytes_transferred);

        if(ec){
            // Failed session::do_write
            std::cerr << "Failed session::do_write! Error Code = " << ec.value()
                      << ". Message: " << ec.message()
                      << '\n';
            return;
        }

        if(close)
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            return do_close();
        }

        // We're done with the response so delete it
        res_ = nullptr;

        // Read another request
        do_read();
    }

    //
    void session::do_close(){

        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    }
}