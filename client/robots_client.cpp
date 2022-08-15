/*
 * SIK 2021/22
 * zad.2 - Bomberman client
 * Michał Napiórkowski
 */

#include <iostream>
#include <cstdlib>
#include <thread>
#include <vector>
#include <algorithm>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include "client.hpp"
#include "types.hpp"

namespace po = boost::program_options;

void print_error(std::string message, char* argv[]) {
    std::cerr << "Error: " << message << std::endl << "Usage: " << argv[0] <<
    " -n <player-name> -p <port> -s <server-address> -d <gui-address>" << std::endl;
    exit(EXIT_FAILURE);
}

params_t parse_parameters(int argc, char* argv[]) {
    params_t params;
    try {
        po::options_description desc("Client options");
        desc.add_options()
                ("help,h", "print help information")
                ("player-name,n", po::value<std::string>(), "set player name")
                ("port,p", po::value<uint16_t>(), "set port")
                ("server-address,s", po::value<std::string>(), "set server address")
                ("gui-address,d", po::value<std::string>(), "set GUI address");
        po::store(po::parse_command_line(argc, argv, desc), params);
        po::notify(params);

        if (params.count("help")) {
            std::cout << desc << std::endl;
            exit(EXIT_SUCCESS);
        }
        if (!params.count("player-name")) {
            print_error("player name not specified", argv);
        }
        if (!params.count("port")) {
            print_error("port not specified", argv);
        }
        if (!params.count("server-address")) {
            print_error("server address not specified", argv);
        }
        if (!params.count("gui-address")) {
            print_error("GUI address not specified", argv);
        }
    } catch (std::exception& e) {
        print_error(e.what(), argv);
    }
    return params;
}

/* Event handlers */

void handle_bomb_placed(Event* event, Client& c) {
    BombPlaced* bomb_placed = dynamic_cast<BombPlaced*>(event);
    std::cout << "             BombPlaced" << std::endl;
    game_info_t game_info = c.get_game_info();
    bomb_info_t bomb_info = {bomb_placed->get_position(), c.get_server_info().bomb_timer};
    game_info.bombs.insert({bomb_placed->get_id(), bomb_info});
    c.set_game_info(game_info);
}

void spread_explosion(game_info_t& game_info, position_t epicenter, position_t vector, position_t edge,
                      uint16_t explosion_radius, set_of_positions_t blocks_destroyed) {
    for (uint16_t i = 0; i <= explosion_radius; i++) {
        position_t pos = {epicenter.first + vector.first * i, epicenter.second + vector.second * i};
        game_info.explosions.insert(pos);
        if (blocks_destroyed.contains(pos) || pos == edge) {
            break;
        }
    }
}

void handle_bomb_exploded(Event* event, Client& c, std::unordered_set<player_id_t>& robots_destroyed) {
    BombExploded* bomb_exploded = dynamic_cast<BombExploded*>(event);
    std::cout << "             BombExploded" << std::endl;
    game_info_t game_info = c.get_game_info();
    server_info_t server_info = c.get_server_info();
    uint16_t explosion_radius = server_info.explosion_radius;
    bomb_id_t bomb_id = bomb_exploded->get_id();
    set_of_positions_t blocks_destroyed = bomb_exploded->get_blocks_destroyed();
    position_t epicenter = game_info.bombs.at(bomb_id).position;

    spread_explosion(game_info, epicenter, {0, 1}, {epicenter.first, server_info.size_y - 1},
                     explosion_radius, blocks_destroyed);
    spread_explosion(game_info, epicenter, {0, -1}, {epicenter.first, 0},
                     explosion_radius, blocks_destroyed);
    spread_explosion(game_info, epicenter, {1, 0}, {server_info.size_x - 1, epicenter.second},
                     explosion_radius, blocks_destroyed);
    spread_explosion(game_info, epicenter, {-1, 0}, {0, epicenter.second},
                     explosion_radius, blocks_destroyed);

    for (auto player_id : bomb_exploded->get_robots_destroyed()) {
        if (!robots_destroyed.contains(player_id)) {
            game_info.scores.at(player_id)++;
            robots_destroyed.insert(player_id);
        }
    }
    for (auto block : blocks_destroyed) {
        game_info.blocks.erase(block);
    }
    game_info.bombs.erase(bomb_id);
    c.set_game_info(game_info);
}

void handle_player_moved(Event* event, Client& c) {
    PlayerMoved* player_moved = dynamic_cast<PlayerMoved*>(event);
    std::cout << "             PlayerMoved" << std::endl;
    game_info_t game_info = c.get_game_info();
    player_id_t id = player_moved->get_id();
    game_info.player_positions[id] = player_moved->get_position();
    c.set_game_info(game_info);
}

void handle_block_placed(Event* event, Client& c) {
    BlockPlaced* block_placed = dynamic_cast<BlockPlaced*>(event);
    std::cout << "             BlockPlaced" << std::endl;
    game_info_t game_info = c.get_game_info();
    game_info.blocks.insert(block_placed->get_position());
    c.set_game_info(game_info);
}

/* ServerMessage handlers */

void handle_hello(ServerMessage* server_mess, Client& c) {
    Hello* hello = dynamic_cast<Hello*>(server_mess);
    std::cout << "<- server:   Hello" << std::endl;
    c.set_server_info(hello->get_server_info());
    Lobby lobby(c.get_server_info(), c.get_game_info().players);
    c.send_to_gui(lobby);
    std::cout << "-> GUI:      Lobby" << std::endl;
}

void handle_accepted_player(ServerMessage* server_mess, Client& c) {
    AcceptedPlayer* accepted_player = dynamic_cast<AcceptedPlayer*>(server_mess);
    std::cout << "<- server:   AcceptedPlayer" << std::endl;
    game_info_t game_info = c.get_game_info();
    player_id_t player_id = accepted_player->get_id();
    game_info.players.insert({player_id, accepted_player->get_player()});
    c.set_game_info(game_info);
    Lobby lobby(c.get_server_info(), c.get_game_info().players);
    c.send_to_gui(lobby);
    std::cout << "-> GUI:      Lobby" << std::endl;
}

void handle_game_started(ServerMessage* server_mess, Client& c) {
    GameStarted* game_started = dynamic_cast<GameStarted*>(server_mess);
    std::cout << "<- server:   GameStarted" << std::endl;
    game_info_t game_info = c.get_game_info();
    game_info.players = game_started->get_players();
    for (auto player : game_info.players) {
        game_info.scores.insert({player.first, 0});
    }
    c.set_game_info(game_info);
}

void handle_turn(ServerMessage* server_mess, Client& c) {
    Turn* turn = dynamic_cast<Turn*>(server_mess);
    std::cout << "<- server:   Turn" << std::endl;
    game_info_t game_info = c.get_game_info();
    game_info.turn = turn->get_turn();
    game_info.explosions.clear();
    for (auto& bomb : game_info.bombs) {
        bomb.second.timer--;
    }
    c.set_game_info(game_info);

    std::vector<Event*> events = turn->get_events();
    std::unordered_set<player_id_t> robots_destroyed;
    for (auto event : events) {
        switch (event->get_type()) {
            case event_t::bomb_placed:
                handle_bomb_placed(event, c);
                break;
            case event_t::bomb_exploded:
                handle_bomb_exploded(event, c, robots_destroyed);
                break;
            case event_t::player_moved:
                handle_player_moved(event, c);
                break;
            case event_t::block_placed:
                handle_block_placed(event, c);
                break;
            default:
                break;
        }
        delete event;
    }
    Game game(c.get_server_info(), c.get_game_info());
    c.send_to_gui(game);
    std::cout << "-> GUI:      Game" << std::endl;
}

void handle_game_ended(ServerMessage* server_mess, Client& c) {
    GameEnded* game_ended = dynamic_cast<GameEnded*>(server_mess);
    std::cout << "<- server:   GameEnded" << std::endl;
    (void)game_ended;
    game_info_t game_info = c.get_game_info();
    game_info.players.clear();
    game_info.player_positions.clear();
    game_info.bombs.clear();
    game_info.blocks.clear();
    game_info.explosions.clear();
    game_info.scores.clear();
    game_info.turn = 0;
    c.set_game_info(game_info);
    Lobby lobby(c.get_server_info(), c.get_game_info().players);
    c.send_to_gui(lobby);
    std::cout << "-> GUI:      Lobby" << std::endl;
}

/* InputMessage handlers */

void handle_input_place_bomb(InputMessage* input_mess, Client& c) {
    InputPlaceBomb* input_place_bomb = dynamic_cast<InputPlaceBomb*>(input_mess);
    std::cout << "<- GUI:      PlaceBomb" << std::endl;
    (void)input_place_bomb;
    PlaceBomb place_bomb;
    c.send_to_server(place_bomb);
    std::cout << "-> server:   PlaceBomb" << std::endl;
}

void handle_input_place_block(InputMessage* input_mess, Client& c) {
    InputPlaceBlock* input_place_block = dynamic_cast<InputPlaceBlock*>(input_mess);
    std::cout << "<- GUI:      PlaceBlock" << std::endl;
    (void)input_place_block;
    PlaceBlock place_block;
    c.send_to_server(place_block);
    std::cout << "-> server:   PlaceBlock" << std::endl;
}

void handle_input_move(InputMessage* input_mess, Client& c) {
    InputMove* input_move = dynamic_cast<InputMove*>(input_mess);
    std::cout << "<- GUI:      Move" << std::endl;
    Move move(input_move->get_direction());
    c.send_to_server(move);
    std::cout << "-> server:   Move" << std::endl;
}

/* Main handlers */

/* Receives messages from server, modifies client's game_info and sends messages to GUI. */
void handle_server_messages(Client& c) {
    while (true) {
        ServerMessage* server_mess = nullptr;
        try {
            server_mess = c.receive_from_server();
        } catch (std::exception& e) {
            delete server_mess;
            throw std::runtime_error(std::string("Error on receiving from server: ") + e.what());
        }
        switch (server_mess->get_type()) {
            case server_mess_t::hello:
                handle_hello(server_mess, c);
                break;
            case server_mess_t::accepted_player:
                handle_accepted_player(server_mess, c);
                break;
            case server_mess_t::game_started:
                handle_game_started(server_mess, c);
                break;
            case server_mess_t::turn:
                handle_turn(server_mess, c);
                break;
            case server_mess_t::game_ended:
                handle_game_ended(server_mess, c);
                break;
            default:
                break;
        }
        delete server_mess;
    }
}

/* Receives input from GUI and sends messages to server */
void handle_input_messages(Client& c) {
    Join join(c.get_player_name());
    while (true) {
        InputMessage* input_mess = nullptr;
        try {
            input_mess = c.receive_from_gui();
        } catch (InvalidMessage& e) {
            delete input_mess;
            continue; // ignore invalid input message
        }
        try {
            switch (input_mess->get_type()) {
                case input_mess_t::input_place_bomb:
                    if (c.is_in_lobby()) {
                        c.send_to_server(join);
                        std::cout << "-> server:   Join" << std::endl;
                    } else {
                        handle_input_place_bomb(input_mess, c);
                    }
                    break;
                case input_mess_t::input_place_block:
                    if (c.is_in_lobby()) {
                        c.send_to_server(join);
                        std::cout << "-> server:   Join" << std::endl;
                    } else {
                        handle_input_place_block(input_mess, c);
                    }
                    break;
                case input_mess_t::input_move:
                    if (c.is_in_lobby()) {
                        c.send_to_server(join);
                        std::cout << "-> server:   Join" << std::endl;
                    } else {
                        handle_input_move(input_mess, c);
                    }
                    break;
                default:
                    break;
            }
        } catch (std::exception& e) {
            delete input_mess;
            std::cerr << "Error on sending to server: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
        delete input_mess;
    }
}

/*
 * Main thread reads only on TCP socket and writes only on UDP,
 * the other reads only on UDP socket and writes only on TCP.
 * Only main thread modifies client.
 */
int main(int argc, char* argv[]) {
    try {
        boost::asio::io_context io_context;
        params_t params = parse_parameters(argc, argv);
        Client c(params, io_context);
        std::thread input_handler(handle_input_messages, std::ref(c));
        input_handler.detach();
        handle_server_messages(c);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
