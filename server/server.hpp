#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <random>
#include <boost/bind/bind.hpp>
#include "connection.hpp"

using boost::asio::ip::tcp;

class Server {
private:
    boost::asio::io_context& io_context;
    tcp::acceptor acceptor;
    std::vector<Connection::pointer> connections;
    std::map<player_id_t, Connection::pointer> player_connections;

    server_info_t server_info;
    uint64_t turn_duration;
    uint16_t initial_blocks;
    std::minstd_rand generator;
    game_info_t game_info;
    player_id_t next_player_id;
    bomb_id_t next_bomb_id;
    std::vector<Turn> turns;
    bool in_lobby;

    /* server.cpp */
    void handle_accept(Connection::pointer new_connection,
                       const boost::system::error_code& error);
    void send_initial_messages(Connection::pointer connection);
    void send_to_everyone(ServerMessage const& mess);
    void close_connection(Connection::pointer connection);

    /* server_game.cpp */
    void start_game();
    void new_turn();
    void handle_turn();
    void end_game();

    /* server_game_helpers.cpp */
    void spawn_player(std::pair<player_id_t, player_info_t> player,
                      std::vector<std::shared_ptr<Event>>& events);
    void spawn_block(std::vector<std::shared_ptr<Event>>& events);
    void spread_explosion(std::pair<bomb_id_t, bomb_info_t> bomb, position_t vector,
                          std::unordered_set<player_id_t>& robots_destroyed,
                          set_of_positions_t& blocks_destroyed);
    void check_bombs(std::unordered_set<player_id_t>& all_robots_destroyed,
                     std::vector<std::shared_ptr<Event>>& events);
    void check_players(std::unordered_set<player_id_t> all_robots_destroyed,
                       std::vector<std::shared_ptr<Event>>& events);

    /* server_receiver.cpp */
    void wait_for_message(Connection::pointer connection);
    void handle_mess_type(Connection::pointer connection,
                          const boost::system::error_code& error,
                          std::size_t bytes_transferred);
    void wait_for_name_length(Connection::pointer connection);
    void wait_for_name(Connection::pointer connection,
                       const boost::system::error_code& error,
                       std::size_t bytes_transferred);
    void handle_join(Connection::pointer connection,
                     const boost::system::error_code& error,
                     std::size_t bytes_transferred);
    void handle_place_bomb(Connection::pointer connection);
    void handle_place_block(Connection::pointer connection);
    void wait_for_direction(Connection::pointer connection);
    void handle_move(Connection::pointer connection,
                     const boost::system::error_code& error,
                     std::size_t bytes_transferred);

    /* server_event_executors.cpp */
    void do_place_bomb(position_t position,
                       std::vector<std::shared_ptr<Event>>& events);
    void do_place_block(position_t position,
                        std::vector<std::shared_ptr<Event>>& events);
    void do_move(position_t position, direction_t direction, player_id_t id,
                 std::vector<std::shared_ptr<Event>>& events);


public:
    Server(params_t params, boost::asio::io_context& io_context);
    void start_accept();
    uint32_t random() { return (uint32_t)generator(); };
};

#endif //SERVER_HPP
