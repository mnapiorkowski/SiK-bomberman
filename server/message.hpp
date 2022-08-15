#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "event.hpp"

class ClientMessage {
protected:
    client_mess_t type;

public:
    ClientMessage() = default;
    ClientMessage(client_mess_t type) :
        type(type) {}
    virtual ~ClientMessage() = default;
    client_mess_t get_type() { return type; }
};

class Join : public ClientMessage {
private:
    std::string name;

public:
    Join() :
        ClientMessage(client_mess_t::join) {}
    Join(std::string name) :
        ClientMessage(client_mess_t::join),
        name(name) {}
    ~Join() = default;
    std::string get_name() { return name; }
};

class PlaceBomb : public ClientMessage {
private:

public:
    PlaceBomb() :
        ClientMessage(client_mess_t::place_bomb) {}
    ~PlaceBomb() = default;
};

class PlaceBlock : public ClientMessage {
private:

public:
    PlaceBlock() :
        ClientMessage(client_mess_t::place_block) {}
    ~PlaceBlock() = default;
};

class Move : public ClientMessage {
private:
    direction_t direction;

public:
    Move() :
        ClientMessage(client_mess_t::move) {}
    Move(direction_t direction) :
        ClientMessage(client_mess_t::move),
        direction(direction) {}
    ~Move() = default;
    direction_t get_direction() { return direction; }
};

class ServerMessage {
protected:
    server_mess_t type;

public:
    ServerMessage(server_mess_t type) :
        type(type) {}
    virtual ~ServerMessage() = default;
    virtual bytes_t serialize() const = 0;
};

class Hello : public ServerMessage {
private:
    server_info_t server_info;

public:
    Hello(server_info_t server_info) :
        ServerMessage(server_mess_t::hello),
        server_info(server_info) {}
    ~Hello() = default;
    bytes_t serialize() const;
};

class AcceptedPlayer : public ServerMessage {
private:
    player_id_t id;
    player_info_t player;

public:
    AcceptedPlayer(player_id_t id, player_info_t player) :
        ServerMessage(server_mess_t::accepted_player),
        id(id),
        player(player) {}
    ~AcceptedPlayer() = default;
    bytes_t serialize() const;
};

class GameStarted : public ServerMessage {
private:
    std::map<player_id_t, player_info_t> players;

public:
    GameStarted(std::map<player_id_t, player_info_t> players) :
        ServerMessage(server_mess_t::game_started),
        players(players) {}
    ~GameStarted() = default;
    bytes_t serialize() const;
};

class Turn : public ServerMessage {
private:
    uint16_t turn;
    std::vector<std::shared_ptr<Event>> events;

public:
    Turn(uint16_t turn, std::vector<std::shared_ptr<Event>> events) :
        ServerMessage(server_mess_t::turn),
        turn(turn),
        events(events) {}
    ~Turn() = default;
    bytes_t serialize() const;
};

class GameEnded : public ServerMessage {
private:
    std::map<player_id_t, score_t> scores;

public:
    GameEnded(std::map<player_id_t, score_t> scores) :
        ServerMessage(server_mess_t::game_ended),
        scores(scores) {}
    ~GameEnded() = default;
    bytes_t serialize() const;
};

#endif //MESSAGE_HPP
