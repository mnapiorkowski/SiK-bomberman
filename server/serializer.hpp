#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <unordered_set>
#include <boost/endian/conversion.hpp>
#include "types.hpp"

class Serializer {
public:
    Serializer() = default;
    ~Serializer() = default;

    template <typename T>
    bytes_t serialize(T x);

    template <typename T1, typename T2>
    bytes_t serialize(std::pair<T1, T2> pair);

    template <typename T>
    bytes_t serialize(std::vector<T> list);

    template <typename K, typename V>
    bytes_t serialize(std::map<K, V> map);

    template <typename T>
    bytes_t serialize(std::unordered_set<T> set);

    bytes_t serialize(std::string str);

    bytes_t serialize(set_of_positions_t set);

    bytes_t serialize(direction_t direction);

    bytes_t serialize(player_info_t player);

    bytes_t serialize(server_info_t server_info);
};

template <typename T>
bytes_t Serializer::serialize(T x) {
    bytes_t bytes(sizeof(T));
    boost::endian::native_to_big_inplace(x);
    std::memcpy(bytes.data(), &x, sizeof(T));
    return bytes;
}

template <typename T1, typename T2>
bytes_t Serializer::serialize(std::pair<T1, T2> pair) {
    bytes_t bytes = serialize(pair.first);
    bytes_t next = serialize(pair.second);
    bytes.insert(bytes.end(), next.begin(), next.end());
    return bytes;
}

template <typename T>
bytes_t Serializer::serialize(std::vector<T> list) {
    bytes_t bytes = serialize((uint32_t)list.size());
    for (auto elem : list) {
        bytes_t elem_ser = serialize(elem);
        bytes.insert(bytes.end(), elem_ser.begin(), elem_ser.end());
    }
    return bytes;
}

template <typename K, typename V>
bytes_t Serializer::serialize(std::map<K, V> map) {
    bytes_t bytes = serialize((uint32_t)map.size());
    for (auto elem: map) {
        bytes_t key_ser = serialize(elem.first);
        bytes_t val_ser = serialize(elem.second);
        bytes.insert(bytes.end(), key_ser.begin(), key_ser.end());
        bytes.insert(bytes.end(), val_ser.begin(), val_ser.end());
    }
    return bytes;
}

template <typename T>
bytes_t Serializer::serialize(std::unordered_set<T> set) {
    std::vector<T> v(set.begin(), set.end());
    bytes_t bytes = serialize(v);
    return bytes;
}

#endif //SERIALIZER_HPP
