//
// Created by Garegin Margaryan on 8/23/18.
//

#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <thread>
#include "src/listener.h"
#include "src/utility.h"

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
using namespace micro_cpp;


int main(int argc, char* argv[]) try {

    // Set interrupt handlers
    utility::set_hook();

    // Check service's command line arguments.
    if(argc!=5) {

        std::cerr << "Usage: micro-cpp <address> <port> <doc_root> <threads>\n"
                  << "Example: micro-cpp 0.0.0.0 8080 . 1\n";
        return EXIT_FAILURE;
    }

    // Retrieve initial config params.
    auto const local_ip = boost::asio::ip::make_address(argv[1]);
    auto const local_port = static_cast<unsigned short>(std::stoi(argv[2]));
    auto const doc_root = std::make_shared<std::string>(argv[3]);
    auto const n_threads = std::max(1,std::stoi(argv[4]));

    // Create boost::asio context with n_threads threads allowed to run simultaneously.
    boost::asio::io_context micro_context{n_threads};

    // Initialize and lunch listening port.
    std::make_shared<micro_cpp::listener>(micro_context
            , boost::asio::ip::tcp::endpoint{local_ip, local_port}
            , doc_root)->run();

    // Run boost I/O service on the requested number of threads.
    micro_context.run();

    return EXIT_SUCCESS;

}  catch (std::exception& e) {

    std::cerr << e.what() << "\n";
}