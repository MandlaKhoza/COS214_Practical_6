#ifndef REQUESTER_H
#define REQUESTER_H

//includes
#include <string>
#include "UserRole.h"

struct Requester
{
    public:
        //members
        std::string userId;
        UserRole role;

        //functions
        Requester(): userId(), role(UserRole::Client){}
        Requester(const std::string& uId, const UserRole  r): userId(uId), role(r){}

         
};


#endif