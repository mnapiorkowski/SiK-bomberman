#include "server.hpp"

Server::Server(params_t params, boost::asio::io_context& io_context) :
        io_context(io_context),
        acceptor(io_context, tcp::endpoint(tcp::v6(), params["port"].as<uint16_t>())) {
    acceptor.set_option(tcp::no_delay(true));
    server_info = {
            params["server-name"].as<std::string>(),
            (uint8_t)params["players-count"].as<uint16_t>(),
            params["size-x"].as<uint16_t>(),
            params["size-y"].as<uint16_t>(),
            params["game-length"].as<uint16_t>(),
            params["explosion-radius"].as<uint16_t>(),
            params["bomb-timer"].as<uint16_t>()
    };
    turn_duration = params["turn-duration"].as<uint64_t>();
    initial_blocks = params["initial-blocks"].as<uint16_t>();
    next_player_id = 0;
    next_bomb_id = 0;
    in_lobby = true;
    if (params.count("seed")) {
        generator.seed(params["seed"].as<uint32_t>());
    } else {
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }
}

void Server::start_accept() {
    Connection::pointer new_connection = Connection::create(io_context);
    acceptor.async_accept(new_connection->get_socket(),
                          boost::bind(&Server::handle_accept, this, new_connection,
                                      boost::asio::placeholders::error));
}

void Server::handle_accept(Connection::pointer new_connection,
                           const boost::system::error_code& error) {
    if (!error) {
        connections.push_back(new_connection);
        send_initial_messages(new_connection);
        wait_for_message(new_connection);
    }
    start_accept();
}

void Server::send_initial_messages(Connection::pointer connection) {
    Hello hello(server_info);
    connection->send(hello);

    if (in_lobby) {
        for (auto player : game_info.players) {
            AcceptedPlayer accepted_player(player.first, player.second);
            connection->send(accepted_player);
        }
    } else {
        GameStarted game_started(game_info.players);
        connection->send(game_started);
        for (auto turn : turns) {
            connection->send(turn);
        }
    }
}

void Server::send_to_everyone(ServerMessage const& mess) {
    for (auto connection : connections) {
        connection->send(mess);
    }
}

void Server::close_connection(Connection::pointer connection) {
    connections.erase(
            std::remove(connections.begin(), connections.end(), connection),
            connections.end());
}
