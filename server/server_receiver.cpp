#include "server.hpp"

void Server::wait_for_message(Connection::pointer connection) {
    connection->receive(1, boost::bind(&Server::handle_mess_type, this, connection,
                                       boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred));
}

void Server::handle_mess_type(Connection::pointer connection,
                              const boost::system::error_code& error,
                              std::size_t /*bytes_transferred*/) {
    if (error) {
        close_connection(connection);
        return;
    }
    uint8_t type = connection->get_read_buffer().at(0);
    switch (type) {
        case (uint8_t)client_mess_t::join:
            wait_for_name_length(connection);
            break;
        case (uint8_t)client_mess_t::place_bomb:
            handle_place_bomb(connection);
            break;
        case (uint8_t)client_mess_t::place_block:
            handle_place_block(connection);
            break;
        case (uint8_t)client_mess_t::move:
            wait_for_direction(connection);
            break;
        default: // incorrect message
            close_connection(connection);
            break;
    }
}

void Server::wait_for_name_length(Connection::pointer connection) {
    connection->receive(1, boost::bind(&Server::wait_for_name, this, connection,
                                       boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred));
}

void Server::wait_for_name(Connection::pointer connection,
                           const boost::system::error_code& error,
                           std::size_t /*bytes_transferred*/) {
    if (error) {
        close_connection(connection);
        return;
    }
    uint8_t length = connection->get_read_buffer().at(0);
    connection->receive(length, boost::bind(&Server::handle_join, this, connection,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
}

void Server::handle_join(Connection::pointer connection,
                         const boost::system::error_code& error,
                         std::size_t bytes_transferred) {
    if (error) {
        close_connection(connection);
        return;
    }
    wait_for_message(connection);
    if (!in_lobby) {
        return;
    }

    bytes_t buffer = connection->get_read_buffer();
    std::string name(buffer.begin(), buffer.begin() + bytes_transferred);

    std::stringstream ss;
    ss << connection->get_socket().remote_endpoint();
    std::string address = ss.str();

    player_info_t player = {name, address};
    if (in_lobby && !connection->joined) {
        connection->joined = true;
        game_info.players.insert({next_player_id, player});
        player_connections.insert({next_player_id, connection});
        AcceptedPlayer accepted_player(next_player_id, player);
        next_player_id++;
        send_to_everyone(accepted_player);

        if (next_player_id == server_info.players_count) {
            start_game();
        }
    }
}

void Server::handle_place_bomb(Connection::pointer connection) {
    wait_for_message(connection);
    if (in_lobby) {
        return;
    }
    PlaceBomb place_bomb;
    connection->set_last_message(std::make_shared<PlaceBomb>(place_bomb));
}

void Server::handle_place_block(Connection::pointer connection) {
    wait_for_message(connection);
    if (in_lobby) {
        return;
    }
    PlaceBlock place_block;
    connection->set_last_message(std::make_shared<PlaceBlock>(place_block));
}

void Server::wait_for_direction(Connection::pointer connection) {
    connection->receive(1, boost::bind(&Server::handle_move, this, connection,
                                       boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred));
}

void Server::handle_move(Connection::pointer connection,
                         const boost::system::error_code& error,
                         std::size_t /*bytes_transferred*/) {
    if (error) {
        close_connection(connection);
        return;
    }
    uint8_t direction = connection->get_read_buffer().at(0);
    if (direction != (uint8_t)direction_t::up &&
        direction != (uint8_t)direction_t::right &&
        direction != (uint8_t)direction_t::down &&
        direction != (uint8_t)direction_t::left) {
        close_connection(connection); // incorrect message
        return;
    }
    wait_for_message(connection);
    if (in_lobby) {
        return;
    }
    Move move((direction_t)direction);
    connection->set_last_message(std::make_shared<Move>(move));
}