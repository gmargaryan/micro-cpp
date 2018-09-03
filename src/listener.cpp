//
// Created by Garegin Margaryan on 8/28/18.
//

#include <iostream>
#include <csignal>
#include "listener.h"
#include "session.h"


namespace micro_cpp {

    //
    listener::listener(boost::asio::io_context& io_cont
            , boost::asio::ip::tcp::endpoint ep
            , std::shared_ptr<std::string const> const& doc_root)
    : acceptor_(io_cont)
    , socket_(io_cont)
    , doc_root_(doc_root) {

        boost::system::error_code ec;

        //Open the Acceptor
        acceptor_.open(ep.protocol(), ec);

        if(ec){
            // Failed to open the acceptor
            std::cerr << "Failed to open the acceptor! Error Code = " << ec.value()
                      << ". Message: " << ec.message()
                      << '\n';
            return;
        }

        // Allow address reuse
        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);

        if(ec){
            // Failed to set an option address reuse
            std::cerr << "Failed to set an option address reuse! Error Code = " << ec.value()
                      << ". Message: " << ec.message()
                      << '\n';
            return;
        }

        // Bind the local address
        acceptor_.bind(ep, ec);

        if(ec){
            // Failed to Bind the local address
            std::cerr << "Failed to Bind the local address! Error Code = " << ec.value()
                      << ". Message: " << ec.message()
                      << '\n';
            return;
        }

        // Start listening for connections
        acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);

        if(ec){
            // Failed to start listening for connections
            std::cerr << "Failed to start listening for connections! Error Code = " << ec.value()
                      << ". Message: " << ec.message()
                      << '\n';
            return;
        }
    }

    //
    void listener::run() {

        if(acceptor_.is_open()) {
            do_accept();
        }
    }

    //
    void listener::do_accept() {

        acceptor_.async_accept(socket_
                , std::bind(&listener::on_accept, shared_from_this(), std::placeholders::_1));
    }

    //
    void listener::on_accept(boost::system::error_code ec) {

        if(ec){
            // Failed to accept connection
            std::cerr << "Failed to accept connection! Error Code = " << ec.value()
                      << ". Message: " << ec.message()
                      << '\n';
        } else {

            // Create new Session and run it
            std::make_shared<micro_cpp::session>(std::move(socket_), doc_root_)->run();
        }

        // Accept another connection
        do_accept();
    }
}
