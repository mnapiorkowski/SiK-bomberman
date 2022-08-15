#include "message.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "receiver.hpp"

/* ClientMessage serializers */

bytes_t Join::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->name);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t PlaceBomb::serialize() const {
    bytes_t bytes({(uint8_t)this->type});
    return bytes;
}

bytes_t PlaceBlock::serialize() const {
    bytes_t bytes({(uint8_t)this->type});
    return bytes;
}

bytes_t Move::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->direction);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

/* ServerMessage receivers and deserializers */

bytes_t ServerMessage::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    bytes_t bytes = r.receive_n_bytes(1);
    server_mess_t first_byte = (server_mess_t)bytes.at(0);
    bytes_t next;
    switch (first_byte) {
        case server_mess_t::hello: {
            Hello hello;
            next = hello.receive(socket);
        }
            break;
        case server_mess_t::accepted_player: {
            AcceptedPlayer accepted_player;
            next = accepted_player.receive(socket);
        }
            break;
        case server_mess_t::game_started: {
            GameStarted game_started;
            next = game_started.receive(socket);
        }
            break;
        case server_mess_t::turn: {
            Turn turn;
            next = turn.receive(socket);
        }
            break;
        case server_mess_t::game_ended: {
            GameEnded game_ended;
            next = game_ended.receive(socket);
        }
            break;
        default:
            break;
    }
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

ServerMessage* ServerMessage::deserialize(bytes_t bytes) {
    ServerMessage* mess_ptr = nullptr;
    server_mess_t first_byte = (server_mess_t)bytes.at(0);
    switch (first_byte) {
        case server_mess_t::hello: {
            Hello* hello = new Hello;
            mess_ptr = hello->deserialize(bytes);
        }
            break;
        case server_mess_t::accepted_player: {
            AcceptedPlayer* accepted_player = new AcceptedPlayer;
            mess_ptr = accepted_player->deserialize(bytes);
        }
            break;
        case server_mess_t::game_started: {
            GameStarted* game_started = new GameStarted;
            mess_ptr = game_started->deserialize(bytes);
        }
            break;
        case server_mess_t::turn: {
            Turn* turn = new Turn;
            mess_ptr = turn->deserialize(bytes);
        }
            break;
        case server_mess_t::game_ended: {
            GameEnded* game_ended = new GameEnded;
            mess_ptr = game_ended->deserialize(bytes);
        }
            break;
        default:
            throw InvalidMessage();
    }
    return mess_ptr;
}

bytes_t Hello::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    server_info_t server_info;
    bytes_t bytes = r.receive(server_info);
    return bytes;
}

ServerMessage* Hello::deserialize(bytes_t bytes) {
    Deserializer d;
    server_info_t server_info;
    d.deserialize(bytes.begin() + 1, server_info);
    this->server_info = server_info;
    return this;
}

bytes_t AcceptedPlayer::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    player_id_t id;
    bytes_t bytes = r.receive(id);
    player_info_t player;
    bytes_t next = r.receive(player);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

ServerMessage* AcceptedPlayer::deserialize(bytes_t bytes) {
    Deserializer d;
    player_id_t id;
    bytes_t::iterator iter = d.deserialize(bytes.begin() + 1, id);
    this->id = id;
    player_info_t player;
    d.deserialize(iter, player);
    this->player = player;
    return this;
}

bytes_t GameStarted::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    std::map<player_id_t, player_info_t> players;
    bytes_t bytes = r.receive(players);
    return bytes;
}

ServerMessage* GameStarted::deserialize(bytes_t bytes) {
    Deserializer d;
    std::map<player_id_t, player_info_t> players;
    d.deserialize(bytes.begin() + 1, players);
    this->players = players;
    return this;
}

bytes_t Turn::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    uint16_t turn;
    bytes_t bytes = r.receive(turn);
    uint32_t size;
    bytes_t next = r.receive(size);
    bytes.insert(bytes.end(), next.begin(), next.end());
    Deserializer d;
    d.deserialize(next.begin(), size);
    Event* event_ptr = new Event;
    for (uint32_t i = 0; i < size; i++) {
        next = event_ptr->receive(socket);
        bytes.insert(bytes.end(), next.begin(), next.end());
    }
    delete event_ptr;
    return bytes;
}

ServerMessage* Turn::deserialize(bytes_t bytes) {
    Deserializer d;
    uint16_t turn;
    bytes_t::iterator iter = d.deserialize(bytes.begin() + 1, turn);
    this->turn = turn;
    uint32_t size;
    iter = d.deserialize(iter, size);
    std::vector<Event*> events;
    Event* event_ptr = new Event;
    try {
        for (uint32_t i = 0; i < size; i++) {
            Event* event = event_ptr->deserialize(iter);
            events.push_back(event);
        }
    } catch (std::exception& e) {
        delete event_ptr;
        throw InvalidMessage();
    }
    delete event_ptr;
    this->events = events;
    return this;
}

bytes_t GameEnded::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    std::map<player_id_t, score_t> scores;
    bytes_t bytes = r.receive(scores);
    return bytes;
}

ServerMessage* GameEnded::deserialize(bytes_t bytes) {
    Deserializer d;
    std::map<player_id_t, score_t> scores;
    d.deserialize(bytes.begin() + 1, scores);
    this->scores = scores;
    return this;
}

/* DrawMessage serializers */

bytes_t Lobby::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->server_info);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->players);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t Game::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->server_name);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->size_x);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->size_y);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->game_length);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->game_info);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

/* InputMessage deserializers */

InputMessage* InputMessage::deserialize(bytes_t bytes) {
    InputMessage* mess_ptr = nullptr;
    if (bytes.empty()) {
        throw InvalidMessage();
    }
    input_mess_t first_byte = (input_mess_t)bytes.at(0);
    switch (first_byte) {
        case input_mess_t::input_place_bomb: {
            InputPlaceBomb* input_place_bomb = new InputPlaceBomb;
            mess_ptr = input_place_bomb->deserialize(bytes);
        }
            break;
        case input_mess_t::input_place_block: {
            InputPlaceBlock* input_place_block = new InputPlaceBlock;
            mess_ptr = input_place_block->deserialize(bytes);
        }
            break;
        case input_mess_t::input_move: {
            InputMove* input_move = new InputMove;
            mess_ptr = input_move->deserialize(bytes);
        }
            break;
        default:
            throw InvalidMessage();
    }
    return mess_ptr;
}

InputMessage* InputPlaceBomb::deserialize(bytes_t bytes) {
    bytes_t::iterator iter = bytes.begin() + 1;
    if (iter != bytes.end()) {
        throw InvalidMessage();
    }
    return this;
}

InputMessage* InputPlaceBlock::deserialize(bytes_t bytes) {
    bytes_t::iterator iter = bytes.begin() + 1;
    if (iter != bytes.end()) {
        throw InvalidMessage();
    }
    return this;
}

InputMessage* InputMove::deserialize(bytes_t bytes) {
    Deserializer d;
    bytes_t::iterator iter = bytes.begin() + 1;
    direction_t direction;
    iter = d.deserialize(iter, direction);
    if (iter != bytes.end() || direction > direction_t::left) {
        throw InvalidMessage();
    }
    this->direction = direction;
    return this;
}