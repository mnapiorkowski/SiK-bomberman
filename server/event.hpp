#ifndef EVENT_HPP
#define EVENT_HPP

#include "types.hpp"

class Event {
protected:
    event_t type;

public:
    Event(event_t type) :
        type(type) {}
    virtual ~Event() = default;
    virtual bytes_t serialize() const = 0;
};

class BombPlaced : public Event {
private:
    bomb_id_t id;
    position_t position;

public:
    BombPlaced(bomb_id_t id, position_t position) :
        Event(event_t::bomb_placed),
        id(id),
        position(position) {}
    ~BombPlaced() = default;
    bytes_t serialize() const;
};

class BombExploded : public Event {
private:
    bomb_id_t id;
    std::unordered_set<player_id_t> robots_destroyed;
    set_of_positions_t blocks_destroyed;

public:
    BombExploded(bomb_id_t id, std::unordered_set<player_id_t> robots_destroyed,
                 set_of_positions_t blocks_destroyed) :
        Event(event_t::bomb_exploded),
        id(id),
        robots_destroyed(robots_destroyed),
        blocks_destroyed(blocks_destroyed) {}
    ~BombExploded() = default;
    bytes_t serialize() const;
};

class PlayerMoved : public Event {
private:
    player_id_t id;
    position_t position;

public:
    PlayerMoved(player_id_t id, position_t position) :
        Event(event_t::player_moved),
        id(id),
        position(position) {}
    ~PlayerMoved() = default;
    bytes_t serialize() const;
};

class BlockPlaced : public Event {
private:
    position_t position;

public:
    BlockPlaced(position_t position) :
        Event(event_t::block_placed),
        position(position) {}
    ~BlockPlaced() = default;
    bytes_t serialize() const;
};

#endif //EVENT_HPP
