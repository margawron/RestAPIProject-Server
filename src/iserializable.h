#ifndef ISERIALIZABLE_H
#define ISERIALIZABLE_H

#include <string>

class ISerializable
{
public:
    virtual ~ISerializable();
    virtual std::string to_json() = 0;
};

#endif // ISERIALIZABLE_H
