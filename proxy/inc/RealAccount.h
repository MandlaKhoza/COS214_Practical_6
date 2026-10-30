#ifndef REALACCOUNT_H
#define REALACCOUNT_H

#include <vector>
#include <string>

#include "AccountView.h"

class Account;


/**
 * @brief RealSubject of the Proxy pattern: the actual account view.
 *
 * Wraps a borrowed Account and shows its details with no access checks.
 * It must only be created by AccountProxy after access has been granted.
 */

class RealAccount : public AccountView
{
    //members
    private:
        /// @brief The account being viewed. Borrowed from AccountRegistry, not owned.
        const Account* account;

        /// @brief Internal staff notes, e.g. "Approved by Branch Manager".
        std::vector<std::string> internalNotes;

    //functions
    public:

        /**
        * @brief Creates a view of an account.
        * @param a      The account to view; must outlive this object.
        * @param notes  Internal notes shown only to staff.
        */
        RealAccount(const Account* a, const std::vector<std::string>& n);

        /**
        * @brief Shows the full account details.
        *   @return The account's description and all of its details.
        */
        std::string display() const override;

        /**
        * @brief Gets the number of the account.
        * @return The account number.
        */ 
        std::string getAccountNumber() const override;

        /**
        * @brief Shows a short summary of the account.
        * @return The account number, type and creation date only.
        */
        std::string displaySummary() const;

        /**
        * @brief Gets the internal notes.
        * @return A reference to the notes; valid while this object exists.
        */
        const std::vector<std::string>& getInternalNotes() const;

        /**
        * @brief Gets the id of the account's owner.
        * @return The owner's id, as stored in the account.
        */
        const std::string& getOwnerId() const; 

};

#endif