//
// Created by Garegin Margaryan on 8/28/18.
//

#ifndef MICRO_CPP_LISTENER_H
#define MICRO_CPP_LISTENER_H

#include <boost/asio/ip/tcp.hpp>


namespace micro_cpp {

    class listener : public std::enable_shared_from_this<listener> {

    private:
        boost::asio::ip::tcp::socket socket_;
        std::shared_ptr<std::string const> doc_root_;
        boost::asio::ip::tcp::acceptor acceptor_;

    public:

        // Constructor
        listener( boost::asio::io_context& cnt
                , boost::asio::ip::tcp::endpoint ep
                , std::shared_ptr<std::string const> const& doc_root);

        //
        void run();
        void do_accept();
        void on_accept(boost::system::error_code ec);

    };
}


#endif //MICRO_CPP_LISTENER_H
