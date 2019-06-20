#include "exception.h"

FileOpenException::FileOpenException(const char * msg):msg_(msg){}

std::string FileOpenException::what() const
{
    return msg_;
}

NotImplementedException::NotImplementedException(const char * msg):msg_(msg){}

std::string NotImplementedException::what() const
{
    return msg_;
}

