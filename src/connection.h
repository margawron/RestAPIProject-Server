#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include <memory>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

#include "databaseadaptor.h"



namespace asio = boost::asio;

class Connection : public std::enable_shared_from_this<Connection>
    /*
     * Dzięki shared_from_this można łączyć operacje w sieci w łańcuchy
     * po wykonaniu których obiekt zostanie automatycznie usunięty
     * inaczej mówiąć pozwala samemu obiektowi zarządzać jego czasem życia
    */
{
    // Obiekt wejścia/wyjścia
    asio::ip::tcp::socket socket_;
    // Wiadomość otrzymana z kanału
    // musi istnieć w całym czasie istnienia połączenia
    // bufor na wiadomość przychodzącą musi być tablicą
    std::array<char, 8192> in_;
    // Wiadomość do wysłania
    std::string out_;
    // Adaptor do bazy danych
    IConnectionAdaptor & adaptor_;

public:

    using pointer = std::shared_ptr<Connection>;

    // Stwórz wskaźnik do połączenia dla danego kanału
    static pointer create(asio::io_context& io,IConnectionAdaptor& adaptor);
    // Zwróć referencje na gniazdo sieciowe
    asio::ip::tcp::socket& socket();
    // funkcja wywołana po otrzymaniu danych od klienta
    void start();
private:
    // konstruktor połączenia dla danego kanału użyty w funkcji create
    Connection(asio::io_context& io_,IConnectionAdaptor & adaptor);
    // odpowiedz na zapytanie HTTP i przejdź do oczekiwania na następne zapytanie
    void handle_read(const boost::system::error_code& error, size_t bytes_transfered);
/* nie używane */
    //     oczekuj na następne pakiety
//    void handle_write(const boost::system::error_code& error, size_t bytes_transfered);
};

#endif // CONNECTION_H
