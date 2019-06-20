#include "http_codes.h"


std::string httpMsg::ec_ok()
{
    std::string response =      "HTTP/1.1 200 OK\r\n";
                response.append("Content-Type: text/html\r\n");
                response.append("Content-Length: 83\r\n\r\n");
                response.append("<html><head><title>200 OK</title></head><body><h1>200 OK</h1></body></html>\r\n\r\n    ");
    return response;
}

std::string httpMsg::ec_base_ok()
{
    std::string response =      "HTTP/1.1 200 OK\r\n";
                response.append("Content-Type: application/json\r\n");
    return response;
}

std::string httpMsg::ec_created()
{
    std::string response =      "HTTP/1.1 201 Created\r\n";
                response.append("Content-Type: text/html\r\n");
                response.append("Content-Length: 85\r\n\r\n");
                response.append("<html><head><title>201 Created</title></head><body><h1>201 Created</h1></body></html>\r\n\r\n    ");
    return response;
}

std::string httpMsg::ec_accepted()
{
    std::string response =      "HTTP/1.1 202 Accepted\r\n";
                response.append("Content-Type: text/html\r\n");
                response.append("Content-Length: 87\r\n\r\n");
                response.append("<html><head><title>202 Accepted</title></head><body><h1>202 Accepted</h1></body></html>\r\n\r\n    ");
    return response;
}

std::string httpMsg::ec_bad_request()
{
    std::string response =      "HTTP/1.1 400 Bad Request\r\n";
                response.append("Content-Type: text/html\r\n");
                response.append("Content-Length: 93\r\n\r\n");
                response.append("<html><head><title>400 Bad Request</title></head><body><h1>400 Bad Request</h1></body></html>\r\n\r\n    ");
    return response;
}

std::string httpMsg::ec_not_found()
{
    std::string response =      "HTTP/1.1 404 Not Found\r\n";
                response.append("Content-Type: text/html\r\n");
                response.append("Content-Length: 97\r\n\r\n");
                response.append("<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1></body></html>\r\n\r\n    ");
    return response;
}

std::string httpMsg::ec_not_acceptable()
{
    std::string response =      "HTTP/1.1 406 Not Acceptable\r\n";
                response.append("Content-Type: text/html\r\n");
                response.append("Content-Length: 99\r\n\r\n");
                response.append("<html><head><title>406 Not Acceptable</title></head><body><h1>406 Not Acceptable</h1></body></html>\r\n\r\n    ");
    return response;
}


std::string httpMsg::ec_conflict()
{
    std::string response =      "HTTP/1.1 409 Conflict\r\n";
                response.append("Content-Type: text/html\r\n");
                response.append("Content-Length: 95\r\n\r\n");
                response.append("<html><head><title>409 Conflict</title></head><body><h1>409 Conflict</h1></body></html>\r\n\r\n    ");
    return response;
}

std::string httpMsg::ec_internal_server_error()
{
    std::string response =      "HTTP/1.1 501 Internal Server Error\r\n";
                response.append("Content-Type: text/html\r\n");
                response.append("Content-Length: 113\r\n\r\n");
                response.append("<html><head><title>501 Internal Server Error</title></head><body><h1>501 Internal Server Error</h1></body></html>\r\n\r\n    ");
    return response;
}

std::string httpMsg::ec_not_implemented()
{
    std::string response =      "HTTP/1.1 502 Not implemented\r\n";
                response.append("Content-Type: text/html\r\n");
                response.append("Content-Length: 110\r\n\r\n");
                response.append("<html><head><title>502 Not implemented</title></head><body><h1>502 Not implemented </h1></body></html>\r\n\r\n    ");
    return response;
}


