#ifndef USER_H
#define USER_H

#include <ctime>
#include <string>
#include <vector>
#include <list>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "iserializable.h"

namespace pt = boost::property_tree;
namespace jason = boost::property_tree::json_parser;


class Event: public ISerializable {
public:
    Event() = default;
    ~Event() override = default;
    // Pola
    unsigned int /* long long */ id_; // const
    int minute_;
    int hour_;
    int day_;
    int month_;
    int year_;
    std::string desc_;
    // Metody
    std::string to_json() override;
};


class User: public ISerializable {
public:
    User() = default;
    ~User() override = default;
    // Pola
    long id_;   // const
    std::string nickname_; // const
    std::string password_;
    long amount_of_events; // mógłby to być int ale kompilator narzeka na padding
    // Metody
    std::string to_json() override;
};

#endif // USER_H
