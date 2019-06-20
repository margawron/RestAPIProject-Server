#ifndef DATABASE_H
#define DATABASE_H

#include <ctime>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <atomic>

#include "user.h"

class Database
{
public:
    // Stałe
    // Folder z danymi bazy danych
    const std::string DB_DIR = "db";
    // Nazwa pliku dla bytu w kolekcji
    const std::string DB_ENT_FILE = "this.cfg";
    // Nazwa katalogu z wydarzeniami
    const std::string DB_EVENT_DIR = "events";

    // Pola
    std::atomic<unsigned int> user_autoinc_;

    // Metody
    Database();
    ~Database();
    // GET
    std::pair<std::shared_ptr<User>,int> get_user(std::string name);
    std::pair<std::shared_ptr<Event>,int> get_user_event(unsigned int id, std::string username);
    // POST
    int create_user(std::string name,std::string password);
    int create_event(std::shared_ptr<User> usr, std::shared_ptr<Event> evnt);
    // PUT
    int update_user(std::shared_ptr<User> user);
    int update_event(std::shared_ptr<User> usr, std::shared_ptr<Event> event);
    // DELETE
    int delete_user(std::string name);
    int delete_event(std::string username, unsigned int user_event_id);

};

#endif // DATABASE_H
