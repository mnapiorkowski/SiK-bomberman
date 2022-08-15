#ifndef DESERIALIZER_HPP
#define DESERIALIZER_HPP

#include <cstring>
#include <vector>
#include <map>
#include <boost/endian/conversion.hpp>
#include "types.hpp"

class Deserializer {
private:

public:
    Deserializer() = default;

    template <typename T>
    bytes_t::iterator deserialize(bytes_t::iterator iter, T& x);

    template <typename T1, typename T2>
    bytes_t::iterator deserialize(bytes_t::iterator iter, std::pair<T1, T2>& pair);

    template <typename T>
    bytes_t::iterator deserialize(bytes_t::iterator iter, std::vector<T>& list);

    template <typename K, typename V>
    bytes_t::iterator deserialize(bytes_t::iterator iter, std::map<K, V>& map);

    bytes_t::iterator deserialize(bytes_t::iterator iter, std::string& str);

    bytes_t::iterator deserialize(bytes_t::iterator iter, set_of_positions_t& set);

    bytes_t::iterator deserialize(bytes_t::iterator iter, direction_t& direction);

    bytes_t::iterator deserialize(bytes_t::iterator iter, player_info_t& player);

    bytes_t::iterator deserialize(bytes_t::iterator iter, bomb_info_t& bomb);

    bytes_t::iterator deserialize(bytes_t::iterator iter, server_info_t& server_info);
};

template <typename T>
bytes_t::iterator Deserializer::deserialize(bytes_t::iterator iter, T& x) {
    size_t size = sizeof(T);
    std::memcpy(&x, &(*iter), size);
    boost::endian::big_to_native_inplace(x);
    std::advance(iter, size);
    return iter;
}

template <typename T1, typename T2>
bytes_t::iterator Deserializer::deserialize(bytes_t::iterator iter, std::pair<T1, T2>& pair) {
    T1 first;
    T2 second;
    iter = deserialize(iter, first);
    iter = deserialize(iter, second);
    pair.first = first;
    pair.second = second;
    return iter;
}

template <typename T>
bytes_t::iterator Deserializer::deserialize(bytes_t::iterator iter, std::vector<T>& list) {
    uint32_t size;
    iter = deserialize(iter, size);
    list.clear();
    for (uint32_t i = 0; i < size; i++) {
        T elem;
        iter = deserialize(iter, elem);
        list.push_back(elem);
    }
    return iter;
}

template <typename K, typename V>
bytes_t::iterator Deserializer::deserialize(bytes_t::iterator iter, std::map<K, V>& map) {
    uint32_t size;
    iter = deserialize(iter, size);
    map.clear();
    for (uint32_t i = 0; i < size; i++) {
        K key;
        V val;
        iter = deserialize(iter, key);
        iter = deserialize(iter, val);
        map.insert({key, val});
    }
    return iter;
}

#endif //DESERIALIZER_HPP
