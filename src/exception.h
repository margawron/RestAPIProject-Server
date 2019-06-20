#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

class IException
{
public:
    virtual std::string what() const = 0;
};

class FileOpenException : public IException{
    std::string msg_;
public:
    FileOpenException(const char * msg);
    std::string what() const;
};

class NotImplementedException : public IException{
    std::string msg_;
public:
    NotImplementedException(const char * msg);
    std::string what() const;
};

#endif // EXCEPTION_H
