#include "message.hpp"
#include "serializer.hpp"

bytes_t Hello::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->server_info);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t AcceptedPlayer::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->id);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize(this->player);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t GameStarted::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->players);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

bytes_t Turn::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->turn);
    bytes.insert(bytes.end(), next.begin(), next.end());
    next = s.serialize((uint32_t)events.size());
    bytes.insert(bytes.end(), next.begin(), next.end());
    for (auto event_ptr : this->events) {
        next = event_ptr->serialize();
        bytes.insert(bytes.end(), next.begin(), next.end());
    }
    return bytes;
}

bytes_t GameEnded::serialize() const {
    Serializer s;
    bytes_t bytes({(uint8_t)this->type});
    bytes_t next = s.serialize(this->scores);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}