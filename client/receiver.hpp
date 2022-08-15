#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <map>
#include <boost/asio.hpp>
#include "types.hpp"
#include "deserializer.hpp"

class Receiver {
private:
    boost::asio::ip::tcp::socket& socket;

public:
    Receiver(boost::asio::ip::tcp::socket& socket) :
        socket(socket) {}

    bytes_t receive_n_bytes(size_t n);

    template <typename T>
    bytes_t receive(T&);

    template <typename T1, typename T2>
    bytes_t receive(std::pair<T1, T2>&);

    template <typename T>
    bytes_t receive(std::vector<T>&);

    template <typename K, typename V>
    bytes_t receive(std::map<K, V>&);

    bytes_t receive(std::string&);

    bytes_t receive(set_of_positions_t&);

    bytes_t receive(player_info_t&);

    bytes_t receive(bomb_info_t&);

    bytes_t receive(server_info_t&);

    bytes_t receive(game_info_t&);
};

template <typename T>
bytes_t Receiver::receive(T&) {
    return receive_n_bytes(sizeof(T));
}

template <typename T1, typename T2>
bytes_t Receiver::receive(std::pair<T1, T2>&) {
    return receive_n_bytes(sizeof(T1) + sizeof(T2));
}

template <typename T>
bytes_t Receiver::receive(std::vector<T>&) {
    uint32_t size;
    bytes_t bytes = receive_n_bytes(sizeof size);
    Deserializer d;
    d.deserialize(bytes.begin(), size);
    for (uint32_t i = 0; i < size; i++) {
        T elem;
        bytes_t next = receive(elem);
        bytes.insert(bytes.end(), next.begin(), next.end());
    }
    return bytes;
}

template <typename K, typename V>
bytes_t Receiver::receive(std::map<K, V>&) {
    uint32_t size;
    bytes_t bytes = receive_n_bytes(sizeof size);
    Deserializer d;
    d.deserialize(bytes.begin(), size);
    for (uint32_t i = 0; i < size; i++) {
        K key;
        V val;
        bytes_t next = receive(key);
        bytes.insert(bytes.end(), next.begin(), next.end());
        next = receive(val);
        bytes.insert(bytes.end(), next.begin(), next.end());
    }
    return bytes;
}

#endif //RECEIVER_HPP
