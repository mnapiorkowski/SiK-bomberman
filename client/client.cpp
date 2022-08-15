#include <iostream>
#include <string>
#include "client.hpp"

host_t host_from_string(std::string str) {
    size_t colon = str.find_last_of(":");
    if (colon == std::string::npos) {
        throw std::invalid_argument("wrong format of address '" + str + "' (expected host:port)");
    }
    host_t host;
    host.address = str.substr(0, colon);
    host.port = str.substr(colon + 1);
    return host;
}

Client::Client(params_t params, boost::asio::io_context& io_context) :
        io_context(io_context),
        udp_socket(io_context),
        tcp_socket(io_context),
        player_name(params["player-name"].as<std::string>()) {
    host_t server = host_from_string(params["server-address"].as<std::string>());
    host_t gui = host_from_string(params["gui-address"].as<std::string>());

    udp_socket.open(udp::v6());
    udp_socket.bind(udp::endpoint(udp::v6(), params["port"].as<uint16_t>()));
    udp::resolver udp_resolver(io_context);
    udp_endpoint = *udp_resolver.resolve(gui.address, gui.port).begin();

    tcp::resolver tcp_resolver(io_context);
    tcp::resolver::results_type endpoints = tcp_resolver.resolve(server.address, server.port);
    boost::asio::connect(tcp_socket, endpoints);
    tcp_socket.set_option(tcp::no_delay(true));
    std::cout << "connected to server: " << tcp_socket.remote_endpoint() << std::endl;
}

void Client::send_to_server(ClientMessage const& mess) {
    boost::asio::write(tcp_socket, boost::asio::buffer(mess.serialize()));
}

ServerMessage* Client::receive_from_server() {
    ServerMessage* mess_ptr = new ServerMessage;
    bytes_t bytes;
    try {
        bytes = mess_ptr->receive(tcp_socket);
    } catch (boost::system::system_error& e) {
        delete mess_ptr;
        throw std::runtime_error("closed connection to server");
    }
    ServerMessage* received = mess_ptr->deserialize(bytes);
    delete mess_ptr;
    return received;
}

void Client::send_to_gui(DrawMessage const& mess) {
    udp_socket.send_to(boost::asio::buffer(mess.serialize()), udp_endpoint);
}

InputMessage* Client::receive_from_gui() {
    udp::endpoint sender_endpoint;
    bytes_t bytes(MAX_UDP_SIZE);
    size_t received_size = udp_socket.receive_from(boost::asio::buffer(bytes), sender_endpoint);
    bytes.resize(received_size);
    InputMessage* mess_ptr = new InputMessage;
    InputMessage* received = mess_ptr->deserialize(bytes);
    delete mess_ptr;
    return received;
}