#include "server.hpp"

void Server::do_place_bomb(position_t position,
                           std::vector<std::shared_ptr<Event>>& events) {
    bomb_info_t bomb_info = {position, server_info.bomb_timer};
    game_info.bombs.insert({next_bomb_id, bomb_info});
    BombPlaced bomb_placed(next_bomb_id, position);
    events.push_back(std::make_shared<BombPlaced>(bomb_placed));
    next_bomb_id++;
}

void Server::do_place_block(position_t position,
                            std::vector<std::shared_ptr<Event>>& events) {
    if (!game_info.blocks.contains(position)) {
        game_info.blocks.insert(position);
        BlockPlaced block_placed(position);
        events.push_back(std::make_shared<BlockPlaced>(block_placed));
    }
}

void Server::do_move(position_t position, direction_t direction, player_id_t id,
                     std::vector<std::shared_ptr<Event>>& events) {
    if ((position.first == 0 && direction == direction_t::left) ||
        (position.first == server_info.size_x - 1 && direction == direction_t::right) ||
        (position.second == 0 && direction == direction_t::down) ||
        (position.second == server_info.size_y - 1 && direction == direction_t::up)) {
        return; // jesteśmy na skraju
    }
    position_t new_position = position;
    switch (direction) {
        case direction_t::right:
            new_position.first++;
            break;
        case direction_t::left:
            new_position.first--;
            break;
        case direction_t::up:
            new_position.second++;
            break;
        case direction_t::down:
            new_position.second--;
            break;
        default:
            break;
    }
    if (!game_info.blocks.contains(new_position)) {
        game_info.player_positions.at(id) = new_position;
        PlayerMoved player_moved(id, new_position);
        events.push_back(std::make_shared<PlayerMoved>(player_moved));
    }
}
