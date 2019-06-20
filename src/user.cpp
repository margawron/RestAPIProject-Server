#include "user.h"

std::string Event::to_json()
{
    pt::ptree json;
    std::string s_ret{};
    std::stringstream ss(s_ret);

    json.add<unsigned>("event_id",id_);
    json.add<int>("event_min",minute_);
    json.add<int>("event_hour",hour_);
    json.add<int>("event_day",day_);
    json.add<int>("event_month",month_);
    json.add<int>("event_year",year_);
    json.add<std::string>("event_desc",desc_);

    jason::write_json(ss,json);
    s_ret = ss.str();
    return s_ret;
}

std::string User::to_json()
{
    pt::ptree json;
    std::string s_ret{};
    std::stringstream ss(s_ret);

    json.add<long>("user_id",id_);
    json.add<std::string>("user_name", nickname_);
    json.add<std::string>("user_password",password_);
    json.add<unsigned>("user_num_of_events", amount_of_events);

    jason::write_json(ss,json);
    s_ret = ss.str();
    return s_ret;
}
