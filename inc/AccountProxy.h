#ifndef ACCOUNTPROXY_H
#define ACCOUNTPROXY_H

#include <memory>
#include <string>

#include "Requester.h"
#include "AccessLevel.h"
#include "AccountView.h"
#include "AccessLog.h"
#include "RealAccount.h"

class AccountRegistry;


/**
 * @brief Proxy of the Proxy pattern: controls who may view an account.
 *
 * On display() it works out the requester's access level, logs the attempt,
 * and only then loads the real account. A denied request never loads it.
 * Access: the owner sees the full account, staff see the full account plus
 * internal notes, auditors see a summary with a masked id, anyone else is denied.
 */
class AccountProxy : public AccountView
{
    //members
    private:
        /// @brief Where accounts are looked up.
        std::shared_ptr<AccountRegistry> registry;

        /// @brief Number of the account being requested.
        std::string accountNumber;

        /// @brief Who is asking.
        Requester requester;

        /// @brief Shared log that records every access attempt.
        std::shared_ptr<AccessLog> log;

        /// @brief The real account, created lazily once access is granted.
        mutable std::unique_ptr<RealAccount> real;

    //functions
    public:
        /**
        * @brief Creates a proxy for one account and one requester.
        * @param r    Registry used to find the account.
        * @param aN   Number of the account to view.
        * @param req  Who is asking.
        * @param l    Log that records the attempt.
        */
        AccountProxy(const std::shared_ptr<AccountRegistry>& r, const std::string& aN, const Requester& req, const std::shared_ptr<AccessLog>& l);

        /**
        * @brief Shows the account according to the requester's access level.
        * @return Full details, full details with notes, a summary, or "Access denied".
        * @note Logs every attempt, including denied ones.
        */
        std::string display() const override;

        /**
        * @brief Gets the number of the requested account.
        * @return The account number.
        */
        std::string getAccountNumber() const override;

    private:
        /**
         @brief Works out what the requester may see.
        * @return Full for the owner, FullWithNotes for staff, Summary for auditors,
        *         Denied for everyone else, including an unknown account.
        */
        AccessLevel determineAccess() const;

        /**
        * @brief Loads the real account on first use.
        * @return The real account view.
        * @warning Call only after access has been granted.
        */
        RealAccount& loadReal() const;

        /**
        * @brief Hides all but the last four characters of an id.
        * @param id  The id to mask.
        * @return The masked id, e.g. "*********9087".
        */
        static std::string maskId(const std::string& id);

};

#endif