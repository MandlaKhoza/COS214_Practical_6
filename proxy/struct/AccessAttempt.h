#ifndef ACCESSATTEMPT_H
#define ACCESSATTEMPT_H

//includes
#include <string>
#include "AccessLevel.h"

/**
 * @brief defines how an attempt to access an account looks like
 */
struct AccessAttempt
{
    public:
        //members
        /// @brief indicates when the access attempt was logged
        std::string timestamp;

        /// @brief indicates the ID of the user that attempted to access the account
        std::string userId;

        /// @brief indicates the number of the account that was trying to be accessed
        std::string accountNumber;

        /// @brief indicates the level of access the user that tried to access the account has
        AccessLevel level;

        //functions
        /**
         * @brief Default constructor that defaults access level to denied
         */
        AccessAttempt() : timestamp(), userId(), accountNumber(), level(AccessLevel::Denied) {}

        /**
         * @brief creates an accessattempt
         * @param ts when the access attempt was logged
         * @param uId ID of user that logged the access attempt
         * @param aNum account number that was trying to be accessed
         * @param l access level of user trying to access the account
         */
        AccessAttempt(const std::string& ts, const std::string& uId, const std::string& aNum, AccessLevel l) : 
            timestamp(ts), userId(uId), accountNumber(aNum), level(l) {}

};

#endif