#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "server.h"
#include "databaseadaptor.h"
#include "database.h"

int main()
{
    Database db{};
    DatabaseAdaptor adaptor(db);

    try {
        boost::asio::io_context io; // Stwórz kontekst w jakim będzie dochodzić do wymiany informacji
        Server server(io,adaptor);  // Utwórz serwer
        std::cout << "Starting server\n";
        // Uruchom kontekst (może zwrócić wyjątek)
        //Użyć - telnet 127.0.0.1 6000 - aby otrzymać wiadomość
        //io.run();
        std::thread t1{[&io](){io.run(); }};
        t1.detach();
        std::cout << "Server started press Enter to exit\n";

        std::cin.get();
        // https://www.boost.org/doc/libs/1_45_0/doc/html/boost_asio/reference/io_service/stop.html
        io.reset();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        io.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    } catch (std::exception& exec) {
        std::cout << exec.what() << "\n";
    }
    return 0;
}
