#ifndef IDATABASEADAPTOR_H
#define IDATABASEADAPTOR_H

#include <string>
#include <vector>

class IConnectionAdaptor
{
public:
    virtual ~IConnectionAdaptor();
    virtual int process_query(std::string & query, std::string & response) = 0;
};

#endif // IDATABASEADAPTOR_H
