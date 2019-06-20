#include "server.h"

Server::Server(boost::asio::io_context& io_,DatabaseAdaptor & adaptor)
    :io_context_(io_),acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),6000)),adaptor_(adaptor)
{
    // Przy utworzeniu serwera uruchom natychmiast połączenie oczekujące
    start_accept();
}

void Server::start_accept(){
    // Utwórz połączenie oczekujące
    Connection::pointer new_connection = Connection::create(io_context_,adaptor_);

    /*
     * Uruchamia funkcję akceptora odpowiadającą za otrzymywanie wiadomości
     * przekazuje do niej gniazdo utworzonego połączenia
     * oraz funkcję jaka ma zostać wywołana po ukończeniu wykonywania połączenia
     * operacja ta jest asynchroniczna
     * */
    acceptor_.async_accept(/* gniazdo */ new_connection->socket(),
                           /* przekaż kontrolę po zaakceptowaniu połączenia */
                           boost::bind(&Server::handle_accept,this,new_connection,
                           boost::asio::placeholders::error));
}


void Server::handle_accept(Connection::pointer new_connection, const boost::system::error_code& error)
{
    // Jeżeli udało się zaakceptować połączenie
    if(!error)
    {
        // Pozwól połączeniu działać
        new_connection->start();
    }
    // Utwórz nowe połączenie
    start_accept();
}
