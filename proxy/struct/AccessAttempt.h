#ifndef ACCESSATTEMPT_H
#define ACCESSATTEMPT_H

//includes
#include <string>
#include "AccessLevel.h"

struct AccessAttempt
{
    public:
        //members
        std::string timestamp;
        std::string userId;
        std::string accountNumber;
        AccessLevel level;

        //functions
        AccessAttempt() : timestamp(), userId(), accountNumber(), level(AccessLevel::Denied) {}
        AccessAttempt(const std::string& ts, const std::string& uId, const std::string& aNum, AccessLevel l) : 
            timestamp(ts), userId(uId), accountNumber(aNum), level(l) {}

};

#endif