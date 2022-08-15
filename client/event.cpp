#include "event.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "receiver.hpp"

bytes_t Event::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    bytes_t bytes = r.receive_n_bytes(1);
    event_t first_byte = (event_t)bytes.at(0);
    bytes_t next;
    switch (first_byte) {
        case event_t::bomb_placed: {
            BombPlaced bomb_placed;
            next = bomb_placed.receive(socket);
        }
            break;
        case event_t::bomb_exploded: {
            BombExploded bomb_exploded;
            next = bomb_exploded.receive(socket);
        }
            break;
        case event_t::player_moved: {
            PlayerMoved player_moved;
            next = player_moved.receive(socket);
        }
            break;
        case event_t::block_placed: {
            BlockPlaced block_placed;
            next = block_placed.receive(socket);
        }
            break;
        default:
            break;
    }
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

Event* Event::deserialize(bytes_t::iterator& iter) {
    Event* event_ptr = nullptr;
    Deserializer d;
    uint8_t first_byte;
    iter = d.deserialize(iter, first_byte);
    event_t event_type = (event_t)first_byte;
    switch (event_type) {
        case event_t::bomb_placed: {
            BombPlaced* bomb_placed = new BombPlaced;
            event_ptr = bomb_placed->deserialize(iter);
        }
            break;
        case event_t::bomb_exploded: {
            BombExploded* bomb_exploded = new BombExploded;
            event_ptr = bomb_exploded->deserialize(iter);
        }
            break;
        case event_t::player_moved: {
            PlayerMoved* player_moved = new PlayerMoved;
            event_ptr = player_moved->deserialize(iter);
        }
            break;
        case event_t::block_placed: {
            BlockPlaced* block_placed = new BlockPlaced;
            event_ptr = block_placed->deserialize(iter);
        }
            break;
        default:
            throw InvalidEvent();
    }
    return event_ptr;
}

bytes_t BombPlaced::serialize() {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->id);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->position);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t BombPlaced::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    bomb_id_t id;
    bytes_t bytes = r.receive(id);
    position_t position;
    bytes_t next = r.receive(position);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

Event* BombPlaced::deserialize(bytes_t::iterator& iter) {
    Deserializer d;
    bomb_id_t id;
    iter = d.deserialize(iter, id);
    this->id = id;
    position_t position;
    iter = d.deserialize(iter, position);
    this->position = position;
    return this;
}

bytes_t BombExploded::serialize() {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->id);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->robots_destroyed);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->blocks_destroyed);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t BombExploded::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    bomb_id_t id;
    bytes_t bytes = r.receive(id);
    std::vector<player_id_t> robots_destroyed;
    bytes_t next = r.receive(robots_destroyed);
    bytes.insert(bytes.end(), next.begin(), next.end());
    std::vector<position_t> blocks_destroyed;
    next = r.receive(blocks_destroyed);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

Event* BombExploded::deserialize(bytes_t::iterator& iter) {
    Deserializer d;
    bomb_id_t id;
    iter = d.deserialize(iter, id);
    this->id = id;
    std::vector<player_id_t> robots_destroyed;
    iter = d.deserialize(iter, robots_destroyed);
    this->robots_destroyed = robots_destroyed;
    set_of_positions_t blocks_destroyed;
    iter = d.deserialize(iter, blocks_destroyed);
    this->blocks_destroyed = blocks_destroyed;
    return this;
}

bytes_t PlayerMoved::serialize() {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->id);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->position);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t PlayerMoved::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    player_id_t id;
    bytes_t bytes = r.receive(id);
    position_t position;
    bytes_t next = r.receive(position);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

Event* PlayerMoved::deserialize(bytes_t::iterator& iter) {
    Deserializer d;
    player_id_t id;
    iter = d.deserialize(iter, id);
    this->id = id;
    position_t position;
    iter = d.deserialize(iter, position);
    this->position = position;
    return this;
}

bytes_t BlockPlaced::serialize() {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->position);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t BlockPlaced::receive(boost::asio::ip::tcp::socket& socket) {
    Receiver r(socket);
    position_t position;
    bytes_t bytes = r.receive(position);
    return bytes;
}

Event* BlockPlaced::deserialize(bytes_t::iterator& iter) {
    Deserializer d;
    position_t position;
    iter = d.deserialize(iter, position);
    this->position = position;
    return this;
}