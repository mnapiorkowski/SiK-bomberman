#ifndef TYPES_HPP
#define TYPES_HPP

#include <unordered_set>
#include <boost/program_options.hpp>

using params_t = boost::program_options::variables_map;
using bytes_t = std::vector<uint8_t>;
using position_t = std::pair<uint16_t, uint16_t>;
using player_id_t = uint8_t;
using bomb_id_t = uint32_t;
using score_t = uint32_t;

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (std::pair<T1, T2> const& pair) const {
        std::size_t h1 = std::hash<T1>()(pair.first);
        std::size_t h2 = std::hash<T2>()(pair.second);
        return h1 ^ h2;
    }
};

using set_of_positions_t = std::unordered_set<position_t, pair_hash>;

typedef struct {
    std::string name;
    std::string address;
} player_info_t;

typedef struct {
    position_t position;
    uint16_t timer;
} bomb_info_t;

typedef struct {
    std::string server_name;
    uint8_t players_count;
    uint16_t size_x;
    uint16_t size_y;
    uint16_t game_length;
    uint16_t explosion_radius;
    uint16_t bomb_timer;
} server_info_t;

typedef struct {
    uint16_t turn;
    std::map<player_id_t, player_info_t> players;
    std::map<player_id_t, position_t> player_positions;
    set_of_positions_t blocks;
    std::map<bomb_id_t, bomb_info_t> bombs;
    set_of_positions_t explosions;
    std::map<player_id_t, score_t> scores;
} game_info_t;

typedef struct {
    std::string address;
    std::string port;
} host_t;

enum direction_t { up, right, down, left };
enum client_mess_t { join, place_bomb, place_block, move };
enum server_mess_t { hello, accepted_player, game_started, turn, game_ended };
enum draw_mess_t { lobby, game };
enum input_mess_t { input_place_bomb, input_place_block, input_move };
enum event_t { bomb_placed, bomb_exploded, player_moved, block_placed };

#endif //TYPES_HPP
