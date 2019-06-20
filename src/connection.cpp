#include "connection.h"

#include <boost/regex.hpp>

using pointer = std::shared_ptr<Connection>;

Connection::Connection(asio::io_context& io_,IConnectionAdaptor& adaptor)
    : socket_(io_),adaptor_(adaptor){}


void Connection::start()
{
    // Wczytaj zapytanie i przekaż kontrolę funkcji handle_read
    socket_.async_read_some(asio::buffer(in_),
                            boost::bind(&Connection::handle_read,shared_from_this(),asio::placeholders::error(),asio::placeholders::bytes_transferred()));
}

void Connection::handle_read(const boost::system::error_code& error, size_t bytes_transfered)
{
    // Sprawdź czy wystąpił błąd
    if(error)
    {
        std::cout << error.message() << "\n bytes transfered" << bytes_transfered << "\n";
        return;
    }
    // Przenieś dane z std::array do std::string
    // TODO usunąć 7k '\0' przy każdym zapytaniu
    //    std::string query(std::begin(in_), std::end(in_),);
    std::string query{};
    query.reserve(bytes_transfered);
    std::copy_n(std::begin(in_),bytes_transfered,std::back_insert_iterator(query));

    // Przetwórz zapytanie i utwórz odpowiedź

    adaptor_.process_query(query,out_);


    socket_.write_some(asio::buffer(out_));

}
//void Connection::handle_write(const boost::system::error_code& error, size_t bytes_transfered)
//{
//    // Sprawdź czy wystąpił błąd
//    if(error)
//    {
//        std::cout << error.message() << "\n bytes transfered" << bytes_transfered << "\n";
//        return;
//    }
//    // Czekaj na następne zapytanie następnie przekaż kontrolę funkcji handle_read
//    socket_.async_read_some(asio::buffer(in_),
//                            boost::bind(&Connection::handle_read,shared_from_this(),asio::placeholders::error(),asio::placeholders::bytes_transferred()));
//}

asio::ip::tcp::socket& Connection::socket()
{
    // Zwróć gniazdo połączenia
    return socket_;
}

pointer Connection::create(asio::io_context& io_,IConnectionAdaptor& adaptor)
{
    // utwórz shared_ptr(boost) na połącznie
    return pointer(new Connection(io_,adaptor) );
}

