#include "server.hpp"

void Server::spawn_player(std::pair<player_id_t, player_info_t> player,
                          std::vector<std::shared_ptr<Event>>& events) {
    position_t position = {random() % server_info.size_x,
                           random() % server_info.size_y};
    game_info.player_positions.insert_or_assign(player.first, position);
    PlayerMoved player_moved(player.first, position);
    events.push_back(std::make_shared<PlayerMoved>(player_moved));
}

void Server::spawn_block(std::vector<std::shared_ptr<Event>>& events) {
    position_t position = {random() % server_info.size_x,
                           random() % server_info.size_y};
    if (!game_info.blocks.contains(position)) {
        game_info.blocks.insert(position);
        BlockPlaced block_placed(position);
        events.push_back(std::make_shared<BlockPlaced>(block_placed));
    }
}

void Server::spread_explosion(std::pair<bomb_id_t, bomb_info_t> bomb, position_t vector,
                              std::unordered_set<player_id_t>& robots_destroyed,
                              set_of_positions_t& blocks_destroyed) {
    for (uint16_t i = 0; i <= server_info.explosion_radius; i++) {
        position_t pos = {bomb.second.position.first + vector.first * i,
                          bomb.second.position.second + vector.second * i};
        for (auto player : game_info.player_positions) {
            if (player.second == pos) {
                robots_destroyed.insert(player.first);
            }
        }
        for (auto block : game_info.blocks) {
            if (block == pos) {
                blocks_destroyed.insert(block);
                return;
            }
        }
    }
}

void Server::check_bombs(std::unordered_set<player_id_t>& all_robots_destroyed,
                         std::vector<std::shared_ptr<Event>>& events) {
    std::unordered_set<bomb_id_t> bombs_exploded;
    set_of_positions_t all_blocks_destroyed;
    for (auto& bomb : game_info.bombs) {
        bomb.second.timer--;
        if (bomb.second.timer == 0) {
            std::unordered_set<player_id_t> robots_destroyed;
            set_of_positions_t blocks_destroyed;
            spread_explosion(bomb, {0, 1}, robots_destroyed, blocks_destroyed);
            spread_explosion(bomb, {0, -1}, robots_destroyed, blocks_destroyed);
            spread_explosion(bomb, {1, 0}, robots_destroyed, blocks_destroyed);
            spread_explosion(bomb, {-1, 0}, robots_destroyed, blocks_destroyed);
            BombExploded bomb_exploded(bomb.first, robots_destroyed, blocks_destroyed);
            events.push_back(std::make_shared<BombExploded>(bomb_exploded));
            all_robots_destroyed.insert(robots_destroyed.begin(), robots_destroyed.end());
            all_blocks_destroyed.insert(blocks_destroyed.begin(), blocks_destroyed.end());
            bombs_exploded.insert(bomb.first);
        }
    }
    for (auto block : all_blocks_destroyed) {
        game_info.blocks.erase(block);
    }
    for (auto id : bombs_exploded) {
        game_info.bombs.erase(id);
    }
}

void Server::check_players(std::unordered_set<player_id_t> robots_destroyed,
                           std::vector<std::shared_ptr<Event>>& events) {
    for (auto player : game_info.players) {
        player_id_t id = player.first;
        if (!robots_destroyed.contains(id)) {
            std::shared_ptr<ClientMessage> last_mess =
                    player_connections.at(id)->get_last_message();
            if (last_mess) {
                position_t position = game_info.player_positions.at(id);
                switch (last_mess->get_type()) {
                    case client_mess_t::place_bomb:
                        do_place_bomb(position, events);
                        break;
                    case client_mess_t::place_block:
                        do_place_block(position, events);
                        break;
                    case client_mess_t::move: {
                        std::shared_ptr<Move> move = std::dynamic_pointer_cast<Move>(last_mess);
                        direction_t direction = move->get_direction();
                        do_move(position, direction, player.first, events);
                    }
                        break;
                    default:
                        break;
                }
            }
        } else { // robot was destroyed
            game_info.scores.at(id)++;
            spawn_player(player, events);
        }
    }
}
