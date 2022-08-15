#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include <cstdint>
#include <string>
#include <map>
#include <memory>
#include <boost/asio.hpp>
#include "types.hpp"
#include "event.hpp"

class InvalidMessage : public std::exception {
public:
    const char* what() const throw() {
        return "invalid message";
    }
};

class ClientMessage {
protected:
    client_mess_t type;

public:
    ClientMessage(client_mess_t type) :
        type(type) {}
    virtual ~ClientMessage() = default;
    client_mess_t get_type() { return type; }
    virtual bytes_t serialize() const = 0;
};

class Join : public ClientMessage {
private:
    std::string name;

public:
    Join(std::string name) :
        ClientMessage(client_mess_t::join),
        name(name) {}
    ~Join() = default;
    bytes_t serialize() const;
};

class PlaceBomb : public ClientMessage {
private:

public:
    PlaceBomb() :
        ClientMessage(client_mess_t::place_bomb) {}
    ~PlaceBomb() = default;
    bytes_t serialize() const;
};

class PlaceBlock : public ClientMessage {
private:

public:
    PlaceBlock() :
        ClientMessage(client_mess_t::place_block) {}
    ~PlaceBlock() = default;
    bytes_t serialize() const;
};

class Move : public ClientMessage {
private:
    direction_t direction;

public:
    Move(direction_t direction) :
        ClientMessage(client_mess_t::move),
        direction(direction) {}
    ~Move() = default;
    bytes_t serialize() const;
};

class ServerMessage {
protected:
    server_mess_t type;

public:
    ServerMessage() = default;
    ServerMessage(server_mess_t type) :
        type(type) {}
    virtual ~ServerMessage() = default;
    server_mess_t get_type() { return type; }
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    ServerMessage* deserialize(bytes_t bytes);
};

class Hello : public ServerMessage {
private:
    server_info_t server_info;

public:
    Hello() :
        ServerMessage(server_mess_t::hello) {}
    Hello(server_info_t server_info) :
        ServerMessage(server_mess_t::hello),
        server_info(server_info) {}
    ~Hello() = default;
    server_info_t get_server_info() { return server_info; }
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    ServerMessage* deserialize(bytes_t bytes);
};

class AcceptedPlayer : public ServerMessage {
private:
    player_id_t id;
    player_info_t player;

public:
    AcceptedPlayer() :
        ServerMessage(server_mess_t::accepted_player) {}
    AcceptedPlayer(player_id_t id, player_info_t player) :
        ServerMessage(server_mess_t::accepted_player),
        id(id),
        player(player) {}
    ~AcceptedPlayer() = default;
    player_id_t get_id() { return id; }
    player_info_t get_player() { return player; }
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    ServerMessage* deserialize(bytes_t bytes);
};

class GameStarted : public ServerMessage {
private:
    std::map<player_id_t, player_info_t> players;

public:
    GameStarted() :
        ServerMessage(server_mess_t::game_started) {}
    GameStarted(std::map<player_id_t, player_info_t> players) :
        ServerMessage(server_mess_t::game_started),
        players(players) {}
    ~GameStarted() = default;
    std::map<player_id_t, player_info_t> get_players() { return players; }
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    ServerMessage* deserialize(bytes_t bytes);
};

class Turn : public ServerMessage {
private:
    uint16_t turn;
    std::vector<Event*> events;

public:
    Turn() :
        ServerMessage(server_mess_t::turn) {}
    Turn(uint16_t turn, std::vector<Event*> events) :
        ServerMessage(server_mess_t::turn),
        turn(turn),
        events(events) {}
    ~Turn() = default;
    uint16_t get_turn() { return turn; }
    std::vector<Event*> get_events() { return events; }
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    ServerMessage* deserialize(bytes_t bytes);
};

class GameEnded : public ServerMessage {
private:
    std::map<player_id_t, score_t> scores;

public:
    GameEnded() :
        ServerMessage(server_mess_t::game_ended) {}
    GameEnded(std::map<player_id_t, score_t> scores) :
        ServerMessage(server_mess_t::game_ended),
        scores(scores) {}
    ~GameEnded() = default;
    std::map<player_id_t, score_t> get_scores() { return scores; }
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    ServerMessage* deserialize(bytes_t bytes);
};

class DrawMessage {
protected:
    draw_mess_t type;

public:
    DrawMessage(draw_mess_t type) :
        type(type) {}
    virtual ~DrawMessage() = default;
    draw_mess_t get_type() { return type; }
    virtual bytes_t serialize() const = 0;
};

class Lobby : public DrawMessage {
private:
    server_info_t server_info;
    std::map<player_id_t, player_info_t> players;

public:
    Lobby(server_info_t server_info, std::map<player_id_t, player_info_t> players) :
        DrawMessage(draw_mess_t::lobby),
        server_info(server_info),
        players(players) {}
    ~Lobby() = default;
    bytes_t serialize() const;
};

class Game : public DrawMessage {
private:
    std::string server_name;
    uint16_t size_x;
    uint16_t size_y;
    uint16_t game_length;
    game_info_t game_info;

public:
    Game(server_info_t server_info, game_info_t game_info) :
        DrawMessage(draw_mess_t::game),
        server_name(server_info.server_name),
        size_x(server_info.size_x),
        size_y(server_info.size_y),
        game_length(server_info.game_length),
        game_info(game_info) {}
    ~Game() = default;
    bytes_t serialize() const;
};

class InputMessage {
protected:
    input_mess_t type;

public:
    InputMessage() = default;
    InputMessage(input_mess_t type) :
        type(type) {}
    virtual ~InputMessage() = default;
    input_mess_t get_type() { return type; }
    InputMessage* deserialize(bytes_t bytes);
};

class InputPlaceBomb : public InputMessage {
private:

public:
    InputPlaceBomb() :
        InputMessage(input_mess_t::input_place_bomb) {}
    ~InputPlaceBomb() = default;
    InputMessage* deserialize(bytes_t bytes);
};

class InputPlaceBlock : public InputMessage {
private:

public:
    InputPlaceBlock() :
        InputMessage(input_mess_t::input_place_block) {}
    ~InputPlaceBlock() = default;
    InputMessage* deserialize(bytes_t bytes);
};

class InputMove : public InputMessage {
private:
    direction_t direction;

public:
    InputMove() :
        InputMessage(input_mess_t::input_move) {}
    InputMove(direction_t direction) :
        InputMessage(input_mess_t::input_move),
        direction(direction) {}
    ~InputMove() = default;
    direction_t get_direction() { return direction; }
    InputMessage* deserialize(bytes_t bytes);
};

#endif //MESSAGE_HPP
