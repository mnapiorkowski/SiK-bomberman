#include <iostream>
#include <boost/bind/bind.hpp>
#include "connection.hpp"

Connection::Connection(boost::asio::io_context& io_context)
    : socket(io_context) {
        read_buffer.resize(BUFF_SIZE);
        last_message = nullptr;
        joined = false;
}

void Connection::handle_write(const boost::system::error_code& /*error*/,
                              std::size_t /*bytes_transferred*/) {
}

void Connection::send(ServerMessage const& mess) {
    write_buffer = mess.serialize();
    boost::asio::async_write(socket, boost::asio::buffer(write_buffer),
                             boost::bind(&Connection::handle_write, shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

void Connection::receive(size_t n_bytes, handler_t handler) {
    boost::asio::async_read(socket, boost::asio::buffer(read_buffer),
                            boost::asio::transfer_exactly(n_bytes), handler);
}