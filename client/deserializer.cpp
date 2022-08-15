#include <string>
#include "deserializer.hpp"

bytes_t::iterator Deserializer::deserialize(bytes_t::iterator iter, std::string& str) {
    size_t length = (size_t)*iter;
    std::advance(iter, 1);
    str.assign(iter, iter + length);
    std::advance(iter, length);
    return iter;
}

bytes_t::iterator Deserializer::deserialize(bytes_t::iterator iter, set_of_positions_t& set) {
    std::vector<position_t> v;
    iter = deserialize(iter, v);
    std::copy(v.begin(), v.end(), std::inserter(set, set.end()));
    return iter;
}

bytes_t::iterator Deserializer::deserialize(bytes_t::iterator iter, direction_t& direction) {
    uint8_t dir = (uint8_t)direction;
    iter = deserialize(iter, dir);
    direction = (direction_t)dir;
    return iter;
}

bytes_t::iterator Deserializer::deserialize(bytes_t::iterator iter, player_info_t& player) {
    iter = deserialize(iter, player.name);
    iter = deserialize(iter, player.address);
    return iter;
}

bytes_t::iterator Deserializer::deserialize(bytes_t::iterator iter, bomb_info_t& bomb) {
    iter = deserialize(iter, bomb.position);
    iter = deserialize(iter, bomb.timer);
    return iter;
}

bytes_t::iterator Deserializer::deserialize(bytes_t::iterator iter, server_info_t& server_info) {
    iter = deserialize(iter, server_info.server_name);
    iter = deserialize(iter, server_info.players_count);
    iter = deserialize(iter, server_info.size_x);
    iter = deserialize(iter, server_info.size_y);
    iter = deserialize(iter, server_info.game_length);
    iter = deserialize(iter, server_info.explosion_radius);
    iter = deserialize(iter, server_info.bomb_timer);
    return iter;
}