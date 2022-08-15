#ifndef EVENT_HPP
#define EVENT_HPP

#include <boost/asio.hpp>
#include "types.hpp"

class InvalidEvent : public std::exception {
public:
    const char* what() const throw() {
        return "invalid event";
    }
};

class Event {
protected:
    event_t type;

public:
    Event() = default;
    Event(event_t type) :
        type(type) {}
    virtual ~Event() = default;
    event_t get_type() { return type; }
    virtual bytes_t serialize() { return bytes_t(); }
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    Event* deserialize(bytes_t::iterator& iter);
};

class BombPlaced : public Event {
private:
    bomb_id_t id;
    position_t position;

public:
    BombPlaced() :
        Event(event_t::bomb_placed) {}
    BombPlaced(bomb_id_t id, position_t position) :
        Event(event_t::bomb_placed),
        id(id),
        position(position) {}
    ~BombPlaced() = default;
    bomb_id_t get_id() { return id; }
    position_t get_position() { return position; }
    bytes_t serialize();
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    Event* deserialize(bytes_t::iterator& iter);
};

class BombExploded : public Event {
private:
    bomb_id_t id;
    std::vector<player_id_t> robots_destroyed;
    set_of_positions_t blocks_destroyed;

public:
    BombExploded() :
        Event(event_t::bomb_exploded) {}
    BombExploded(bomb_id_t id, std::vector<player_id_t> robots_destroyed,
                 set_of_positions_t blocks_destroyed) :
        Event(event_t::bomb_exploded),
        id(id),
        robots_destroyed(robots_destroyed),
        blocks_destroyed(blocks_destroyed) {}
    ~BombExploded() = default;
    bomb_id_t get_id() { return id; }
    std::vector<player_id_t> get_robots_destroyed() { return robots_destroyed; }
    set_of_positions_t get_blocks_destroyed() { return blocks_destroyed; }
    bytes_t serialize();
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    Event* deserialize(bytes_t::iterator& iter);
};

class PlayerMoved : public Event {
private:
    player_id_t id;
    position_t position;

public:
    PlayerMoved() :
        Event(event_t::player_moved) {}
    PlayerMoved(player_id_t id, position_t position) :
        Event(event_t::player_moved),
        id(id),
        position(position) {}
    ~PlayerMoved() = default;
    player_id_t get_id() { return id; }
    position_t get_position() { return position; }
    bytes_t serialize();
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    Event* deserialize(bytes_t::iterator& iter);
};

class BlockPlaced : public Event {
private:
    position_t position;

public:
    BlockPlaced() :
        Event(event_t::block_placed) {}
    BlockPlaced(position_t position) :
        Event(event_t::block_placed),
        position(position) {}
    ~BlockPlaced() = default;
    position_t get_position() { return position; }
    bytes_t serialize();
    bytes_t receive(boost::asio::ip::tcp::socket& socket);
    Event* deserialize(bytes_t::iterator& iter);
};

#endif //EVENT_HPP
