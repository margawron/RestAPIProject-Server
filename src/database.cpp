#include "database.h"
#include "exception.h"

#include <ctime>

namespace fs = boost::filesystem;

Database::Database()
{
    // Ścieżka do pliku na jakim dana funckcja bazy danych będzie operować
    std::string this_function_dir(DB_DIR + "/");

    // Jeżeli folder bazy danych nie istnieje
    if(!fs::exists(DB_DIR))
    {
        // Stwórz go
        fs::create_directory(DB_DIR);
        // Zainicjalizuj zmienne
        user_autoinc_ = 1;
        // Stwórz folder konfiguracyjny
        std::fstream db_cfg_file(this_function_dir + DB_ENT_FILE, std::ios::out);
        // Jeżeli nie można stworzyć folderu
        if(!db_cfg_file.good())
        {
            // Rzuć wyjątkiem
            throw FileOpenException("Nie mozna otworzyc pliku z konfiguracja serwera");
        }
        // Zapisz zmienne
        db_cfg_file << user_autoinc_;
        // Zamknij strumien
        db_cfg_file.close();
        // Zakończ
        return;
    }
    // Folder istnieje
    // Otwórz plik
    std::fstream db_cfg_file(this_function_dir + DB_ENT_FILE,std::ios::in);
    // Wczytaj zmienne
    unsigned int tmp_inc;
    db_cfg_file >> tmp_inc;
    user_autoinc_.store(tmp_inc);
    // Zamknij i zakończ
    db_cfg_file.close();

}

Database::~Database()
{
    // W domyśle nikt nie usunął katalogu bazy danych podczas działania programu
    // Otwórz plik konfiguracji bazy danych
    std::fstream db_cfg_file(DB_DIR + "/" + DB_ENT_FILE,std::ios::out);
    // Zapisz zmienne
    db_cfg_file << user_autoinc_ << "\n";
    // Zakończ
    db_cfg_file.close();
}
std::pair<std::shared_ptr<User>,int> Database::get_user(std::string name)
{
    /* check_user_cache() */
    std::transform(name.begin(),name.end(),name.begin(),tolower);
    std::string this_function_dir(DB_DIR + "/" + name + "/");
    // Jeżeli użytkownik nie istnieje zwróć nullptr
    if(!fs::exists(this_function_dir))
    {
        return std::pair(nullptr,404);
    }
    std::fstream user_cfg_file(this_function_dir + "this.cfg" ,std::ios::in);
    // Jeżeli otwarcie pliku nie było możliwe
    // (brak miejsca, brak uprawnien do odczytu)
    // zwróć nullptr
    if(!user_cfg_file.good())
    {
        return std::pair(nullptr,500);
    }
    // Wczytaj dane użytkownika
    std::shared_ptr<User> user =std::make_shared<User>();
    user_cfg_file >> user->id_;
    user_cfg_file >> user->nickname_;
    user_cfg_file >> user->password_;
    user_cfg_file >> user->amount_of_events;
    // Zwróć wskaźnik na użytkownika
    return std::pair(user,200);
}

std::pair<std::shared_ptr<Event>,int> Database::get_user_event(unsigned int id, std::string name)
{
    std::string this_function_dir(DB_DIR + "/" + name + "/" + DB_EVENT_DIR + "/" + std::to_string(id) + ".txt");
    if(!fs::exists(this_function_dir))
    {
        return std::pair(nullptr,404);
    }
    std::fstream event_file(this_function_dir, std::ios::in);
    if(!event_file.good())  // Nie można otworzyć pliku
    {
        return std::pair(nullptr,500);
    }
    std::shared_ptr<Event> ret_val = std::make_shared<Event>();
    event_file >> ret_val->id_;
    event_file >> ret_val->minute_;
    event_file >> ret_val->hour_;
    event_file >> ret_val->day_;
    event_file >> ret_val->month_;
    event_file >> ret_val->year_;
    event_file >> std::noskipws;
    ret_val->desc_ = std::string(std::istream_iterator<char>(event_file),std::istream_iterator<char>());
    event_file.close();
    return std::pair(ret_val,200);
}

int Database::create_user(std::string name,std::string password)
{
    std::string this_function_dir(DB_DIR + "/" + name + "/");
    // Jeżeli użytkownik istnieje
    if(fs::exists(this_function_dir))
    {
        // Błąd użytkownik istnieje - HTTP  409 Conflict
        return 409;
    }
    // Stwórz katalog użytkownika
    fs::create_directory(this_function_dir);
    // Stwórz podkatalog z wydarzeniami
    fs::create_directory(this_function_dir + DB_EVENT_DIR + "/");
    // Stwórz plik z danymi użytkownika
    std::fstream user_cfg_file(this_function_dir + DB_ENT_FILE, std::ios::out);
    // Zmień imię użytkownika do małych liter
    std::transform(name.begin(), name.end(),name.begin(),tolower);
    // Zapisz dane użytkownika
    user_cfg_file << user_autoinc_ << "\n" << name << "\n" << password << "\n" << 0 << "\n";
    // Zwiększ zmienną mówiącą o liczbie użytkowników
    user_autoinc_++;
    // Zamknij strumień wyjściowy
    user_cfg_file.close();
    // HTTP 201 Created
    return 201;
}

int Database::create_event(std::shared_ptr<User> usr,std::shared_ptr<Event> evnt)
{
    std::string this_function_dir(DB_DIR + "/" + usr->nickname_ + "/" + DB_EVENT_DIR + "/");
    if(!fs::exists(this_function_dir))
    {
        // Katalog z wydarzeniami tego użytkownika nie istnieje
        // HTTP 404 Not Found
        return 404;
    }
    if(fs::exists(this_function_dir + std::to_string(evnt->id_) + ".txt"))
    {
        // Te wydarzenie już istnieje
        // HTTP 409 Conflict
        return 409;
    }
    // Nadpisz dane w pliku
    std::fstream event_file(this_function_dir + std::to_string(usr->amount_of_events) + ".txt", std::ios::out);
    event_file << evnt->id_ << "\n";
    event_file << evnt->minute_ << "\n";
    event_file << evnt->hour_ << "\n";
    event_file << evnt->day_ << "\n";
    event_file << evnt->month_ << "\n";
    event_file << evnt->year_;
    event_file << evnt->desc_ << "\n";
    event_file.close();
    // HTTP 201 Created
    return 201;
}

int Database::update_user(std::shared_ptr<User> user)
{
    std::string this_function_dir(DB_DIR + "/" + user->nickname_ + "/");
    if(!fs::exists(this_function_dir))
    {
        // Użytkownik nie istnieje

        return 404;
    }
    auto [existing,_] = get_user(user->nickname_);
    std::fstream out(this_function_dir + DB_ENT_FILE, std::ios::out);
    out << existing->id_<< "\n" << existing->nickname_ << "\n" << user->password_ << "\n" << user->amount_of_events << "\n";
    out.close();
    // HTTP 200 OK
    return 202;
}

int Database::update_event(std::shared_ptr<User> usr, std::shared_ptr<Event> event)
{
    std::string this_function_dir(DB_DIR + "/" + usr->nickname_ + "/" + DB_EVENT_DIR + "/" + std::to_string(event->id_) + ".txt");
    if(!fs::exists(this_function_dir))
    {
        // Wydarzenie nie istnieje
        return 404;
    }
    std::fstream out(this_function_dir, std::ios::out);
    out << event->id_ << "\n";
    out << event->minute_ << "\n";
    out << event->hour_ << "\n";
    out << event->day_ << "\n";
    out << event->month_ << "\n";
    out << event->year_;
    out << (event->desc_) << "\n";
    out.close();
    // HTTP 202 Accepted
    return 202;
}

int Database::delete_user(std::string name)
{
    std::string this_function_dir(DB_DIR + "/" + name + "/");
    if(!fs::exists(this_function_dir))
    {
        // HTTP 404 Not Found
        return 404;
    }
    fs::remove_all(this_function_dir);
    if(!fs::exists(this_function_dir))
    {
        // HTTP 200 OK
        return 200;
    }
    std::cout << "Nie udało się usunąć użytkownika :" + name << "\n";
    // HTTP 500 Internal server error
    return 500;
}

int Database::delete_event(std::string username, unsigned int user_event_id)
{
    std::string this_function_dir(DB_DIR + "/" + username + "/" + DB_EVENT_DIR +"/");
    if(!fs::exists(this_function_dir))
    {
        // HTTP 404 Not Found
        return 404;
    }
    fs::remove(this_function_dir + std::to_string(user_event_id) + ".txt");
    if(!fs::exists(this_function_dir + std::to_string(user_event_id) + ".txt"))
    {
        // HTTP 200 OK
        return 200;
    }
    std::cout << "Nie udało się usunąć wydarzenia nr: " << user_event_id << "\nDla użytkownika " << username << "\n";
    return 500;
}


