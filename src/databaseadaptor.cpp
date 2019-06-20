#include <string>
#include <iterator>
#include <vector>
#include <regex>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "databaseadaptor.h"
#include "exception.h"
#include "http_codes.h"

namespace pt = boost::property_tree;
namespace jason = boost::property_tree::json_parser;

DatabaseAdaptor::DatabaseAdaptor(Database& db):db_(db){}

int DatabaseAdaptor::process_query(std::string &query, std::string &response)
{
    // Podzielone zapytania
    // Korzystamy tylko z 0 pozycji vectora
    std::vector<std::string> splited_query{};
    split_query(query,splited_query,std::regex("\r\n"));
    if(splited_query.size() < 1)
    {
        response = httpMsg::ec_bad_request();
        return -1;
    }
    std::vector<std::string> params{};
    split_query(splited_query[0],params,std::regex("( |\\0)"));
    if(params.size() < 3)
    {
        response = httpMsg::ec_bad_request();
        return -1;
    }

    // Wektor z parsowanym URI
    std::vector<std::string> tokenized_uri{};
    split_query(params[1],tokenized_uri,std::regex("/"));

    // Lekceważymy pozostałą część nagłówka klienta poza
    // danymi po podwójnej (\r\n\r\n)
    if(params[0] == "GET"){
        return process_get_request(tokenized_uri,params,response);
    }else if(params[0] == "POST"){
        // Dodaj elementy po pustej linii (\r\n\r\n)
        insert_arguments(splited_query,params);
        return process_post_request(tokenized_uri,params,response);
    }else if(params[0] == "HEAD"){
        return process_head_request(tokenized_uri,params,response);
    }else if(params[0] == "PUT"){
        // Dodaj elementy po pustej linii (\r\n\r\n)
        insert_arguments(splited_query,params);
        return process_put_request(tokenized_uri,params,response);
    }else if(params[0] == "DELETE") {
        return process_delete_request(tokenized_uri,params,response);
    }else {
        response = httpMsg::ec_not_implemented();
        return -1;
    }
}

int DatabaseAdaptor::process_get_request(std::vector<std::string>& uri,std::vector<std::string> & request, std::string & response)
{
    //
    std::string uname{};
    unsigned event_id;
    // Jeżeli uri ma mniej niż 4 elementy to znaczy że zapytanie nie jest poprawne
    if(uri.size() < 4)
    {
        response = httpMsg::ec_not_found();
        return -1;
    }
    // Jeżeli zapytanie ma 4 elementy należy sprawdzić składowe uri
    // Możliwe że jest to zapytanie do kolekcji users
    else if(uri.size() == 4)
    {
        // Sprawdzanie czy składowe uri się zgadzają
        if(!(uri[1] == "api" && uri[2] == "users"))
        {
            // Nie zgadzają się
            response = httpMsg::ec_not_found();
            return -1;
        }
        // Zgadzają się
        uname = uri[3]; // nazwa użytkownika jest 4 elementem uri
        // Pobierz użytkownika z bazy danych i kod odpowiedzi
        auto [user,status] = db_.get_user(uname);
        if(status == 200)  // Pobranie danych przeszło pomyślnie
        {
            // Zamień obiekt na plik JSON
            std::string json = user->to_json();
            // Bazowy kod 200
            response = httpMsg::ec_base_ok();
            // Dodaj długość json'a
            response.append("Content-Length: " + std::to_string(json.length()));
            response.append("\r\n\r\n");
            // Dodaj json'a na koniec
            response.append(json);
            return 0;
        }else if (status == 404) { // Nie ma takiego użytkownika w bazie danych
            response = httpMsg::ec_not_found();
            return -1;
        }else if (status == 500) {  // Nie udało się otworzyć pliku z danymi
            response = httpMsg::ec_internal_server_error();
            return -1;
        }else {                     // Nie craszuj serwera
            response = httpMsg::ec_internal_server_error();
            return -1;
        }
    }
    // Jeżeli zapytanie ma 6 elementy należy sprawdzić składowe uri
    // Możliwe że jest to zapytanie do podkolekcji event kolekcji users
    else if (uri.size() == 6) {
        if(!(uri[1] == "api" && uri[2] == "users" && uri[4] == "events"))
        {
            // składowe uri się nie zgadzają
            response = httpMsg::ec_not_found();
            return -1;
        }
        // nazwa użytkownika jest na 4 pozycji wektora uri
        uname = uri[3];
        try { // ID eventu jest liczbą, sprawdź czy użytkownik nie wprowadził tekstu
            event_id = static_cast<unsigned>(std::stoi(uri[5])); // dokonaj próby konwersji
        } catch (std::exception& e) { // Złap wyjątek jeżeli nie udało się skonwertować
            std::cout << request[1] << "\n" << e.what() << "\n";
            response = httpMsg::ec_not_found();
            return -1;
        }
        // Pobierz wydarzenie użytkownika i kod odpowiedzi
        auto [event,status] = db_.get_user_event(event_id, uname);
        if(status == 200)
        {
            // Utwórz bufor wraz z plikiem JSON
            std::string json = event->to_json();
            // Konstrukcja wiadomości zwrotnej
            response = httpMsg::ec_base_ok();
            response.append("Content-Length: " + std::to_string(json.length()));
            response.append("\r\n\r\n");
            response.append(json);
            return 0;
        }else if (status == 404) {
            response = httpMsg::ec_not_found();
            return -1;
        }else if (status == 500) {
            response = httpMsg::ec_internal_server_error();
            return -1;
        }else {
            response = httpMsg::ec_internal_server_error();
            return -1;
        }
    }
    // W każdym innym wypadku uri jest nie poprawne
    response = httpMsg::ec_not_found();
    return -1;
}

int DatabaseAdaptor::process_post_request(std::vector<std::string>& uri,std::vector<std::string> & request, std::string & response)
{
    std::string uname{};
    if(uri.size() < 3)
    {
        response = httpMsg::ec_not_found();
        return -1;
    }
    else if (uri.size() == 3 || uri.size() == 4)
    {
        if(!(uri[1] == "api" && uri[2] == "users"))
        {
            // Nie zgadzają się
            response = httpMsg::ec_not_found();
            return -1;
        }
        // Sprawdzanie czy składowe uri się zgadzają
        if(!(uri[1] == "api" && uri[2] == "users"))
        {
            // Nie zgadzają się
            response = httpMsg::ec_not_found();
            return -1;
        }

        std::string json;
        for(unsigned i = 4;i < request.size(); i++)
        {
            json.append(request[i]);
        }
        pt::ptree tree;
        std::stringstream ss(json);
        try{
            jason::read_json(ss, tree);

        }catch(pt::ptree_error& e)
        {
            std::cout << "Could not parse JSON\n" << e.what() << "\n";
            response = httpMsg::ec_bad_request();
            return -1;
        }


        int state;
        try{
            state = db_.create_user(tree.get<std::string>("user_name",uname),tree.get<std::string>("user_password"));
        }catch(pt::ptree_error& e)
        {
            std::cout << "User did not provided necessery information\n" << e.what() << "\n";
            response = httpMsg::ec_bad_request();
            return -1;
        }
        if(state == 201)
        {
            auto [user,_] = db_.get_user(uname);
            auto user_str = user->to_json();
            response = httpMsg::ec_base_ok();
            response.append("Content-Length: " + std::to_string(user_str.length()));
            response.append("\r\n\r\n");
            response.append(user_str);
            return 0;
        }else if (state == 409){
            response = httpMsg::ec_conflict();
            return -1;
        }else {
            response = httpMsg::ec_internal_server_error();
            return -1;
        }
    }
    else if (uri.size() == 6 || uri.size() == 5) {
        if(!(uri[1] == "api" && uri[2] == "users" && uri[4] == "events"))
        {
            // składowe uri się nie zgadzają
            response = httpMsg::ec_not_found();
            return -1;
        }
        // nazwa użytkownika jest na 4 pozycji wektora uri
        uname = uri[3];

        std::string json;
        for(unsigned i = 4;i < request.size(); i++)
        {
            json.append(request[i]);
        }
        pt::ptree tree;
        std::stringstream ss(json);
        try {
            jason::read_json(ss, tree);
        } catch (jason::json_parser_error& e) {
            std::cout << "POST: " << uname << "\n" << json  << "\n\n";
            std::cout << e.what() << "\n";
            response = httpMsg::ec_bad_request();
            return -1;
        }


        int state;
        auto [user, _] = db_.get_user(uname);
        if(user == nullptr)
        {
            response = httpMsg::ec_not_found();
            return -1;
        }
        auto event = std::make_shared<Event>();
        try {
            event->id_ = user->amount_of_events;
            event->minute_ = tree.get<int>("event_min");
            event->hour_ = tree.get<int>("event_hour");
            event->day_ = tree.get<int>("event_day");
            event->month_ = tree.get<int>("event_month");
            event->year_ = tree.get<int>("event_year");
            event->desc_ = tree.get<std::string>("event_desc","");
            state = db_.create_event(user,event);

        } catch (pt::ptree_error& e) {
            std::cout << "User did not provided necessery information\n" << e.what() << "\n";
            response = httpMsg::ec_bad_request();
            return -1;
        }
        if(state == 201)
        {
            user->amount_of_events++;
            auto event_str = event->to_json();
            db_.update_user(user);
            response = httpMsg::ec_base_ok();
            response.append("Content-Length: " + std::to_string(event_str.length()));
            response.append("\r\n\r\n");
            response.append(event_str);
            return 0;
        }else if (state == 404) {
            response = httpMsg::ec_not_found();
            return -1;
        }else if (state == 409) {
            response = httpMsg::ec_conflict();
            return -1;
        }else{
            response = httpMsg::ec_internal_server_error();
            return -1;
        }
    }
    response = httpMsg::ec_not_found();
    return -1;
}

int DatabaseAdaptor::process_head_request(std::vector<std::string>& uri,std::vector<std::string> & request, std::string & response){
    //
    std::string uname{};
    unsigned event_id;
    // Jeżeli uri ma mniej niż 4 elementy to znaczy że zapytanie nie jest poprawne
    if(uri.size() < 4)
    {
        response = httpMsg::ec_not_found();
        return -1;
    }
    // Jeżeli zapytanie ma 4 elementy należy sprawdzić składowe uri
    // Możliwe że jest to zapytanie do kolekcji users
    else if(uri.size() == 4)
    {
        // Sprawdzanie czy składowe uri się zgadzają
        if(!(uri[1] == "api" && uri[2] == "users"))
        {
            // Nie zgadzają się
            response = httpMsg::ec_not_found();
            return -1;
        }
        // Zgadzają się
        uname = uri[3]; // nazwa użytkownika jest 4 elementem uri
        auto [user,status] = db_.get_user(uname);
        if(status == 200)  // Pobranie danych przeszło pomyślnie
        {
            // Zamień obiekt na plik JSON
            std::string json = user->to_json();
            // Bazowy kod 200
            response = httpMsg::ec_base_ok();
            // Dodaj długość json'a
            response.append("Content-Length: " + std::to_string(json.length()));
            response.append("\r\n\r\n");
            // Dodaj json'a na koniec
            return 0;
        }else if (status == 404) { // Nie ma takiego użytkownika w bazie danych
            response = httpMsg::ec_not_found();
            return -1;
        }else if (status == 500) {  // Nie udało się otworzyć pliku z danymi
            response = httpMsg::ec_internal_server_error();
            return -1;
        }else {                     // Nie craszuj serwera
            response = httpMsg::ec_internal_server_error();
            return -1;
        }
    }
    // Jeżeli zapytanie ma 6 elementy należy sprawdzić składowe uri
    // Możliwe że jest to zapytanie do podkolekcji event kolekcji users
    else if (uri.size() == 6) {
        if(!(uri[1] == "api" && uri[2] == "users" && uri[4] == "events"))
        {
            // składowe uri się nie zgadzają
            response = httpMsg::ec_not_found();
            return -1;
        }
        // nazwa użytkownika jest na 4 pozycji wektora uri
        uname = uri[3];
        try { // ID eventu jest liczbą, sprawdź czy użytkownik nie wprowadził tekstu
            event_id = static_cast<unsigned>(std::stoi(uri[5])); // dokonaj próby konwersji
        } catch (std::exception& e) { // Złap wyjątek jeżeli nie udało się skonwertować
            std::cout << request[1] << "\n" << e.what() << "\n";
            response = httpMsg::ec_not_found();
            return -1;
        }
        // Pobierz wydarzenie użytkownika i kod odpowiedzi
        auto [event,status] = db_.get_user_event(event_id, uname);
        if(status == 200)
        {
            // Utwórz bufor wraz z plikiem JSON
            std::string json = event->to_json();
            // Konstrukcja wiadomości zwrotnej
            response = httpMsg::ec_base_ok();
            response.append("Content-Length: " + std::to_string(json.length()));
            response.append("\r\n\r\n");
            return 0;
        }else if (status == 404) {
            response = httpMsg::ec_not_found();
            return -1;
        }else if (status == 500) {
            response = httpMsg::ec_internal_server_error();
            return -1;
        }else {
            response = httpMsg::ec_internal_server_error();
            return -1;
        }
    }
    // W każdym innym wypadku uri jest nie poprawne
    response = httpMsg::ec_not_found();
    return -1;
}
int DatabaseAdaptor::process_put_request(std::vector<std::string>& uri,std::vector<std::string> & request, std::string & response)
{
    std::string uname;
    unsigned event_id;
    if(uri.size() < 4)
    {
        response = httpMsg::ec_not_found();
        return -1;
    }
    else if (uri.size() == 4) { // to może być kolekcja użytkownika
        if(!(uri[1] == "api" && uri[2] == "users"))
        {
            // To nie jest kolekcja użytkownika
            response = httpMsg::ec_not_found();
            return -1;
        }
        // To jest kolekcja użytkownika
        uname = uri[3];
        auto [user,status] = db_.get_user(uname);
        if(user == nullptr)
        {
            response = httpMsg::ec_not_found();
            return -1;
        }
        std::string json;
        for(unsigned i = 4;i < request.size(); i++)
        {
            json.append(request[i]);
        }
        pt::ptree tree;
        std::stringstream ss(json);
        try{
            jason::read_json(ss, tree);

        }catch(pt::ptree_error& e)
        {
            std::cout << "Could not parse JSON\n" << e.what() << "\n";
            response = httpMsg::ec_bad_request();
            return -1;
        }
        int state;

        try{
            user->password_ = tree.get<std::string>("user_password",user->password_);
            //user->amount_of_events = tree.get<long>("user_num_of_events", user->amount_of_events);
            state = db_.update_user(user);
        }catch (pt::ptree_error& e){    // Nie powinno rzucić wyjątkiem, podane są dane domyślne
            std::cout << "Something went wrong during PUT operation on the user: " << user->nickname_ << "\n" << e.what() << "\n";
            response = httpMsg::ec_bad_request();
            return -1;
        }
        if(state == 404)
        {
            response = httpMsg::ec_not_found();
            return -1;
        }else if (state == 202) {
            response = httpMsg::ec_accepted();
            return 0;
        }else {
            response = httpMsg::ec_internal_server_error();
            return -1;
        }
    }
    else if (uri.size() == 6) { // to może być podkolekcja wydarzenia
        if(!(uri[1] == "api" && uri[2] == "users" && uri[4] == "events"))
        {
            // składowe uri się nie zgadzają
            response = httpMsg::ec_not_found();
            return -1;
        }
        // nazwa użytkownika jest na 4 pozycji wektora uri
        uname = uri[3];
        try { // ID eventu jest liczbą, sprawdź czy użytkownik nie wprowadził tekstu
            event_id = static_cast<unsigned>(std::stoi(uri[5])); // dokonaj próby konwersji
        } catch (std::exception& e) { // Złap wyjątek jeżeli nie udało się skonwertować
            std::cout << request[1] << "\n" << e.what() << "\n";
            response = httpMsg::ec_not_found();
            return -1;
        }
        std::string json;
        for(unsigned i = 4;i < request.size(); i++)
        {
            json.append(request[i]);
        }
        pt::ptree tree;
        std::stringstream ss(json);
        try{
            jason::read_json(ss, tree);
        }catch(jason::json_parser_error& e)
        {
            std::cout << "Exception thrown while parsing PUT request on event collection\n" << e.what() << "\n";
            response = httpMsg::ec_internal_server_error();
            return -1;
        }
        auto [user,_] = db_.get_user(uname);
        if(user == nullptr)
        {
            httpMsg::ec_not_found();
            return -1;
        }

        std::shared_ptr<Event> event;

        int state;
        try {

            auto [event_capture,_] = db_.get_user_event(tree.get<long>("event_id",event_id),user->nickname_);
            event = event_capture;
        } catch (pt::ptree_error& e) {  // Nie powinno rzucić wyjątkiem
            std::cout << "User did not provided necessery information\n" << e.what() << "\n";
            response = httpMsg::ec_bad_request();
            return -1;
        }
        if(event == nullptr)
        {
            response = httpMsg::ec_not_found();
            return -1;
        }
        event->minute_ = tree.get<int>("event_min",event->minute_);
        event->hour_ = tree.get<int>("event_hour",event->hour_);
        event->day_ = tree.get<int>("event_hour", event->day_);
        event->month_ = tree.get<int>("event_month",event->month_);
        event->year_ = tree.get<int>("event_year",event->year_);
        event->desc_ = tree.get<std::string>("event_desc",event->desc_);
        state = db_.update_event(user,event);
        // Nie ma potrzeby updateować użytkownika,
        // modyfikujemy tylko event
        if(state == 202)
        {
            response = httpMsg::ec_accepted();
            return 0;
        }else if (state == 404) {
            response = httpMsg::ec_not_found();
            return -1;
        }else {
            response = httpMsg::ec_internal_server_error();
            return -1;
        }
    }
    response = httpMsg::ec_not_found();
    return -1;
}
int DatabaseAdaptor::process_delete_request(std::vector<std::string>& uri,std::vector<std::string> & request, std::string & response)
{
    std::string uname;
    unsigned event_id;
    if(uri.size() < 4) // za mało argumentów w ścieżce
    {
        response = httpMsg::ec_not_found();
        return -1;
    }
    else if (uri.size() == 4) // odpowiednia ilość argumentów dla kolekcji użytkownika
    {
        if(!(uri[1] == "api" && uri[2] == "users")) // sprawdź czy to jest kolekcja użytkownika
        {
            // To nie jest kolekcja użytkownika
            response = httpMsg::ec_not_found();
            return -1;
        }
        // To jest kolekcja użytkownika
        uname = uri[3];
        auto [user,_] = db_.get_user(uname);
        if(user == nullptr) // Czy użytkownik istnieje
        {
            response = httpMsg::ec_not_found();
            return -1;
        }

        int state = db_.delete_user(user->nickname_);
        if(state == 200)
        {
            response = httpMsg::ec_ok();
            return 0;
        }
        else if (state == 404) {
            response = httpMsg::ec_not_found();
            return -1;
        } else if (state == 500) {
            response = httpMsg::ec_internal_server_error();
            return -1;
        } else {
            response = httpMsg::ec_internal_server_error();
            std::cout << "\nServer shouldnt execute code here\nUsername: " << uname <<  "\n\n";
            return -1;
        }
    }
    else if (uri.size() == 6) // odpowiednia ilość argumentów dla podkolekcji wydarzeń
    {
        if(!(uri[1] == "api" && uri[2] == "users" && uri[4] == "events")) // Sprawrdź czy to podkolekcja wydarzeń
        {
            // to nie jest kolekcja wydarzeń
            response = httpMsg::ec_not_found();
            return -1;
        }
        uname = uri[3];
        try { // ID eventu jest liczbą, sprawdź czy użytkownik nie wprowadził tekstu
            event_id = static_cast<unsigned>(std::stoi(uri[5])); // dokonaj próby konwersji
        } catch (std::exception& e) { // Złap wyjątek jeżeli nie udało się skonwertować
            std::cout << request[1] << "\n" << e.what() << "\n";
            response = httpMsg::ec_not_found();
            return -1;
        }
        auto [user,_] = db_.get_user(uname);
        if(user == nullptr)
        {
            response = httpMsg::ec_not_found();
            return -1;
        }
        auto [event,_1] = db_.get_user_event(event_id,user->nickname_);
        if(event == nullptr)
        {
            response = httpMsg::ec_not_found();
            return -1;
        }
        int state = db_.delete_event(user->nickname_,event->id_);
        if(state == 200)
        {
            response = httpMsg::ec_ok();
            return 0;
        }
        else if(state == 404)
        {
            response = httpMsg::ec_not_found();
            return -1;
        }
        else if (state == 500)
        {
            response = httpMsg::ec_internal_server_error();
            return -1;
        }
        else {
            response = httpMsg::ec_internal_server_error();
            std::cout << "\nServer shouldnt execute code here\nUsername: " << uname << "Event id: " << event_id << "\n\n";
            return -1;
        }

    }
    response = httpMsg::ec_not_found();
    return -1;
}

// Podziel zapytanie na części
void DatabaseAdaptor::split_query(std::string & query, std::vector<std::string>& splited, std::regex delim)
{
    // Wyrażenie regularne jakim należy podzielić łańcuch znaków(zapytanie)
    // Kopiuj dane
    std::copy(std::sregex_token_iterator(query.begin(),query.end(),delim,-1)
              /* Iterator po całym zapytaniu który wybiera wszystko po między trafieniami */,
              std::sregex_token_iterator()
              /* Iterator końca sekwencji */,
              std::back_insert_iterator<std::vector<std::string>>(splited)
              /* Iterator początka kontenera do którego mają zostać zapisywane trafienia*/);
}

void DatabaseAdaptor::insert_arguments(std::vector<std::string>& full_request, std::vector<std::string>& param_request)
{
    bool after_empty_line = false;
    for(auto arg : full_request)
    {
        if(arg == "")
        {
            after_empty_line = true;
        }
        if(after_empty_line)
        {
            param_request.push_back(arg);
        }
    }
}
