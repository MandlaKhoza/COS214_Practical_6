#ifndef ACCOUNTVIEW_H
#define ACCOUNTVIEW_H

#include <string>

/**
 * @brief subject interface for the proxy pattern
 * RealAccount and AccountProxy implement it, so TUI can treat them identically and never knows wether access control happens
 */
class AccountView
{
    //functions
    public:
        /// @brief virtual destructor, so a derived object is destroyed correctly through an AccountView pointer
        virtual ~AccountView() = default;

        /// @brief Produces the text the TUI shows for the account.
        /// @return The account details, or “Access denied” if the requester isn’t allowed to see the account. The amount of detail depends on the requester’s access level.
        virtual std::string display() const = 0;

        /// @brief Gets the number of the account being reviewed 
        /// @return The account number
        virtual std::string getAccountNumber() const = 0;

};

#endif