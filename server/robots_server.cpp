/*
 * SIK 2021/22
 * zad.2 - Bomberman server
 * Michał Napiórkowski
 */

#include <iostream>
#include "server.hpp"

namespace po = boost::program_options;

void print_error(std::string message, char* argv[]) {
    std::cerr << "Error: " << message << std::endl << "Usage: " << argv[0] <<
    " -n <server-name> -p <port> -c <players-count> -l <game-length>"
    " -d <turn-duration> -x <size-x> -y <size-y> -k <initial-blocks>"
    " -b <bomb-timer> -e <explosion-radius>" << std::endl;
    exit(EXIT_FAILURE);
}

params_t parse_parameters(int argc, char* argv[]) {
    params_t params;
    try {
        po::options_description desc("Server options");
        desc.add_options()
                ("help,h", "print help information")
                ("server-name,n", po::value<std::string>(), "set server name")
                ("port,p", po::value<uint16_t>(), "set port")
                ("players-count,c", po::value<uint16_t>(), "set number of players")
                ("game-length,l", po::value<uint16_t>(), "set game length (in turns)")
                ("turn-duration,d", po::value<uint64_t>(), "set turn duration (in ms)")
                ("size-x,x", po::value<uint16_t>(), "set board width")
                ("size-y,y", po::value<uint16_t>(), "set board height")
                ("initial-blocks,k", po::value<uint16_t>(), "set number of initial blocks")
                ("bomb-timer,b", po::value<uint16_t>(), "set bomb timer (in turns)")
                ("explosion-radius,e", po::value<uint16_t>(), "set explosion radius")
                ("seed,s", po::value<uint32_t>(), "set RNG seed");
        po::store(po::parse_command_line(argc, argv, desc), params);
        po::notify(params);

        if (params.count("help")) {
            std::cout << desc << std::endl;
            exit(EXIT_SUCCESS);
        }
        if (!params.count("server-name")) {
            print_error("server name not specified", argv);
        }
        if (!params.count("port")) {
            print_error("port not specified", argv);
        }
        if (!params.count("players-count")) {
            print_error("number of players not specified", argv);
        }
        if (!params.count("game-length")) {
            print_error("game length not specified", argv);
        }
        if (!params.count("turn-duration")) {
            print_error("turn duration not specified", argv);
        }
        if (!params.count("size-x")) {
            print_error("board width not specified", argv);
        }
        if (!params.count("size-y")) {
            print_error("board height not specified", argv);
        }
        if (!params.count("initial-blocks")) {
            print_error("number of initial blocks not specified", argv);
        }
        if (!params.count("bomb-timer")) {
            print_error("bomb timer not specified", argv);
        }
        if (!params.count("explosion-radius")) {
            print_error("explosion radius not specified", argv);
        }
        if (params["players-count"].as<uint16_t>() == 0) {
            print_error("number of players must be greater than zero", argv);
        }
        if (params["game-length"].as<uint16_t>() == 0) {
            print_error("game length must be greater than zero", argv);
        }
        if (params["turn-duration"].as<uint64_t>() == 0) {
            print_error("turn duration must be greater than zero", argv);
        }
        if (params["size-x"].as<uint16_t>() == 0) {
            print_error("board width must be greater than zero", argv);
        }
        if (params["size-y"].as<uint16_t>() == 0) {
            print_error("board height must be greater than zero", argv);
        }
        if (params["bomb-timer"].as<uint16_t>() == 0) {
            print_error("bomb timer must be greater than zero", argv);
        }
    } catch (std::exception& e) {
        print_error(e.what(), argv);
    }
    return params;
}

int main(int argc, char* argv[]) {
    try {
        boost::asio::io_context io_context;
        params_t params = parse_parameters(argc, argv);
        Server s(params, io_context);
        s.start_accept();
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
