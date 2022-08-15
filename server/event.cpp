#include "event.hpp"
#include "serializer.hpp"

bytes_t BombPlaced::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->id);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->position);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t BombExploded::serialize() const {
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

bytes_t PlayerMoved::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->id);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->position);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t BlockPlaced::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->position);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}