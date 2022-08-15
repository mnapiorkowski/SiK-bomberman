#include "server.hpp"

void Server::start_game() {
    in_lobby = false;
    GameStarted game_started(game_info.players);
    send_to_everyone(game_started);
    game_info.turn = 0;

    std::vector<std::shared_ptr<Event>> events;
    for (auto player : game_info.players) {
        game_info.scores.insert({player.first, 0});
        spawn_player(player, events);
    }
    for (uint16_t i = 0; i < initial_blocks; i++) {
        spawn_block(events);
    }

    Turn turn(game_info.turn, events);
    turns.push_back(turn);
    send_to_everyone(turn);
    new_turn();
}

void Server::new_turn() {
    game_info.turn++;
    io_context.restart();
    std::chrono::milliseconds duration(turn_duration);
    io_context.run_for(duration);
    handle_turn();
}

void Server::handle_turn() {
    std::vector<std::shared_ptr<Event>> events;
    std::unordered_set<player_id_t> robots_destroyed;
    check_bombs(robots_destroyed, events);
    check_players(robots_destroyed, events);
    for (auto connection : connections) {
        connection->set_last_message(nullptr);
    }

    Turn turn(game_info.turn, events);
    turns.push_back(turn);
    send_to_everyone(turn);
    if (game_info.turn < server_info.game_length) {
        new_turn();
    } else {
        end_game();
    }
}

void Server::end_game() {
    in_lobby = true;
    next_player_id = 0;
    next_bomb_id = 0;
    turns.clear();
    player_connections.clear();
    game_info.players.clear();
    game_info.player_positions.clear();
    game_info.blocks.clear();
    game_info.bombs.clear();
    game_info.explosions.clear();
    game_info.scores.clear();

    for (auto connection : connections) {
        connection->joined = false;
    }
    io_context.restart();
    GameEnded game_ended(game_info.scores);
    send_to_everyone(game_ended);
}
