#ifndef HTTP_CODES_H
#define HTTP_CODES_H

#include <string>

class httpMsg
{
public:
    // Kody HTTP
    // 200 OK
    static std::string ec_ok();
    // 200 OK
    // Należy dodać długość wysyłanej zawartości
    static std::string ec_base_ok();
    // 201 Created
    static std::string ec_created();
    // 202 Accepted
    static std::string ec_accepted();
    // 400 Bad request
    static std::string ec_bad_request();
    // 404 Not Found
    static std::string ec_not_found();
    // 406 Not Acceptable
    static std::string ec_not_acceptable();
    // 409 Conflict
    static std::string ec_conflict();
    // 500 Internal server error
    static std::string ec_internal_server_error();
    // 501 Not implemented
    static std::string ec_not_implemented();
};

#endif // HTTP_CODES_H
