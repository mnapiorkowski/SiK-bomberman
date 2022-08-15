#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "message.hpp"

#define BUFF_SIZE 256

using boost::asio::ip::tcp;
using handler_t = boost::function<void(const boost::system::error_code&, std::size_t)>;

class Connection : public boost::enable_shared_from_this<Connection> {
private:
    tcp::socket socket;
    bytes_t write_buffer;
    bytes_t read_buffer;
    std::shared_ptr<ClientMessage> last_message;

    Connection(boost::asio::io_context& io_context);
    void handle_write(const boost::system::error_code& error,
                      std::size_t /*bytes_transferred*/);

public:
    bool joined;

    typedef boost::shared_ptr<Connection> pointer;
    static pointer create(boost::asio::io_context& io_context) {
        return pointer(new Connection(io_context));
    }

    tcp::socket& get_socket() { return socket; }
    bytes_t get_read_buffer() { return read_buffer; }
    std::shared_ptr<ClientMessage> get_last_message() { return last_message; }
    void set_last_message(std::shared_ptr<ClientMessage> mess_ptr) {
        last_message = mess_ptr;
    }

    void send(ServerMessage const& mess);
    void receive(size_t n_bytes, handler_t handler);
};

#endif //CONNECTION_HPP
