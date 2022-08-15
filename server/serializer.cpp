#include "serializer.hpp"

bytes_t Serializer::serialize(std::string str) {
    if (str.size() > std::numeric_limits<uint8_t>::max()) {
        str = str.substr(0, std::numeric_limits<uint8_t>::max());
    }
    size_t size = str.size();
    bytes_t bytes(size + 1);
    bytes.at(0) = (uint8_t)size;
    if (size > 0) {
        std::memcpy(&(bytes.at(1)), str.data(), size);
    }
    return bytes;
}

bytes_t Serializer::serialize(set_of_positions_t set) {
    std::vector<position_t> v(set.begin(), set.end());
    bytes_t bytes = serialize(v);
    return bytes;
}

bytes_t Serializer::serialize(player_info_t player) {
    bytes_t bytes = serialize(player.name);
    bytes_t next = serialize(player.address);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t Serializer::serialize(server_info_t server_info) {
    bytes_t bytes = serialize(server_info.server_name);
    bytes_t next = serialize(server_info.players_count);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = serialize(server_info.size_x);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = serialize(server_info.size_y);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = serialize(server_info.game_length);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = serialize(server_info.explosion_radius);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = serialize(server_info.bomb_timer);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}