#include "serializer.hpp"

template <typename K, typename V>
std::vector<V> vector_from_map(std::map<K, V> map) {
    std::vector<V> v;
    for (auto elem : map) {
        v.push_back(elem.second);
    }
    return v;
}

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

bytes_t Serializer::serialize(direction_t direction) {
    bytes_t bytes = serialize((uint8_t)direction);
    return bytes;
}

bytes_t Serializer::serialize(player_info_t player) {
    bytes_t bytes = serialize(player.name);
    bytes_t next = serialize(player.address);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t Serializer::serialize(bomb_info_t bomb) {
    bytes_t bytes = serialize(bomb.position);
    bytes_t next = serialize(bomb.timer);
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


bytes_t Serializer::serialize(game_info_t game_info) {
    bytes_t bytes = serialize(game_info.turn);
    bytes_t next = serialize(game_info.players);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = serialize(game_info.player_positions);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = serialize(game_info.blocks);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = serialize(vector_from_map(game_info.bombs));
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = serialize(game_info.explosions);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = serialize(game_info.scores);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}