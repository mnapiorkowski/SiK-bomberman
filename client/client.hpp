#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <boost/asio.hpp>
#include "types.hpp"
#include "message.hpp"

#define MAX_UDP_SIZE 65527

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class Client {
private:
    boost::asio::io_context& io_context;
    udp::endpoint udp_endpoint;
    udp::socket udp_socket;
    tcp::socket tcp_socket;

    std::string player_name;
    server_info_t server_info;
    game_info_t game_info;

public:
    Client(params_t params, boost::asio::io_context& io_context);
    ~Client() = default;
    std::string get_player_name() { return player_name; }
    game_info_t get_game_info() { return game_info; }
    void set_game_info(game_info_t new_info) { game_info = new_info; }
    server_info_t get_server_info() { return server_info; }
    void set_server_info(server_info_t new_info) { server_info = new_info; }
    void send_to_server(ClientMessage const& mess);
    ServerMessage* receive_from_server();
    void send_to_gui(DrawMessage const& mess);
    InputMessage* receive_from_gui();
    bool is_in_lobby() { return game_info.player_positions.empty(); }
};

#endif //CLIENT_HPP
