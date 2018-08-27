#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>


int main(int argc, char* argv[]) {

    // check service's command line arguments
    if(argc!=5) {

        std::cerr << "Usage: micro-cpp <address> <port> <doc_root> <threads>\n"
                  << "Example: micro-cpp 0.0.0.0 . 8080 1\n";
        return EXIT_FAILURE;
    }

    // retrieve initial config params
    auto const local_ip = boost::asio::ip::make_address(argv[1]);
    auto const loacl_port = static_cast<ushort>(std::stoi(argv[2]));
    auto const doc_root = std::string{argv[3]};
    auto const n_threads = std::max(1,std::stoi(argv[4]));


    return 0;
}