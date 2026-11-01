#ifndef REQUESTER_H
#define REQUESTER_H

//includes
#include <string>
#include "UserRole.h"

/**
 * @brief Contains info on the user trying to access the account
 */
struct Requester
{
    public:
        //members
        /// @brief ID of user trying to access the account
        std::string userId;

        /// @brief user role of user trying to access the account
        UserRole role;

        //functions
        /***
         * @brief default constructor that sets the default role to client
         */
        Requester(): userId(), role(UserRole::Client) {}

        /**
         * @brief creates a requester
         * @param uId ID of user trying to access the account
         * @param r Role of the user trying to access the account
         */
        Requester(const std::string& uId, const UserRole  r) : userId(uId), role(r){}

         
};


#endif