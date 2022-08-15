#include "receiver.hpp"

bytes_t Receiver::receive_n_bytes(size_t n) {
    bytes_t bytes(n);
    boost::asio::read(socket, boost::asio::buffer(bytes));
    return bytes;
}

bytes_t Receiver::receive(std::string&) {
    uint8_t length;
    bytes_t bytes = receive_n_bytes(sizeof length);
    Deserializer d;
    d.deserialize(bytes.begin(), length);
    bytes_t next = receive_n_bytes(length);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t Receiver::receive(set_of_positions_t& set) {
    std::vector<position_t> v(set.begin(), set.end());
    bytes_t bytes = receive(v);
    return bytes;
}

bytes_t Receiver::receive(player_info_t& player) {
    bytes_t bytes = receive(player.name);
    bytes_t next = receive(player.address);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t Receiver::receive(bomb_info_t& bomb) {
    bytes_t bytes = receive(bomb.position);
    bytes_t next = receive(bomb.timer);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t Receiver::receive(server_info_t& server_info) {
    bytes_t bytes = receive(server_info.server_name);
    bytes_t next = receive(server_info.players_count);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = receive(server_info.size_x);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = receive(server_info.size_y);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = receive(server_info.game_length);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = receive(server_info.explosion_radius);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = receive(server_info.bomb_timer);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t Receiver::receive(game_info_t& game_info) {
    bytes_t bytes = receive(game_info.turn);
    bytes_t next = receive(game_info.players);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = receive(game_info.player_positions);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = receive(game_info.blocks);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = receive(game_info.bombs);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = receive(game_info.explosions);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = receive(game_info.scores);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}