#ifndef SERWER_H
#define SERWER_H

#include <boost/asio.hpp>

#include "connection.h"
#include "databaseadaptor.h"



class Server{
    // Kanał do porozumiewania się z systemem operacyjnym
    boost::asio::io_context& io_context_;
    // Klasa odpowiadająca za akceptowanie połączeń protokołu tcp
    boost::asio::ip::tcp::acceptor acceptor_;
public:
    // Adapter do bazy danych
    IConnectionAdaptor & adaptor_;

    // Konstruuje serwer w kontekstcie tcp(ipv4) na porcie 6000
    Server(boost::asio::io_context& io_,DatabaseAdaptor& adaptor_);
private:
    // Utwórz połączenie oczekujące na wiadomość
    void start_accept();
    // Działanie po zaakceptowaniu połączenia
    void handle_accept(Connection::pointer new_connection, const boost::system::error_code& error);
};

#endif // SERWER_H
