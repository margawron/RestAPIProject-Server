#ifndef DATABASEADAPTOR_H
#define DATABASEADAPTOR_H

#include <string>
#include <regex>

#include "database.h"
#include "iconnectionadaptor.h"




class DatabaseAdaptor: public IConnectionAdaptor{
    Database& db_;
    /* Funkcje przetwarzające */
    int process_get_request(std::vector<std::string>& uri,std::vector<std::string> & request, std::string & response);
    int process_post_request(std::vector<std::string>& uri,std::vector<std::string> & request, std::string & response);
    int process_head_request(std::vector<std::string>& uri,std::vector<std::string> & request, std::string & response);
    int process_put_request(std::vector<std::string>& uri,std::vector<std::string> & request, std::string & response);
    int process_delete_request(std::vector<std::string>& uri,std::vector<std::string> & request, std::string & response);

    /*  Funkcje pomocnicze */
    // Podziel zapytanie za pomocą wyrażenia regularnego
    void split_query(std::string & query, std::vector<std::string>& splited, std::regex delim);
    // Połącz parametry po pustej linii zapytania
    void insert_arguments(std::vector<std::string>& full_request, std::vector<std::string>& param_request);

public:
    //
   DatabaseAdaptor(Database& db);
   int process_query(std::string & query, std::string & response) override;
};

#endif // DATABASEADAPTOR_H
