#ifndef ACCESSLOG_H
#define ACCESSLOG_H

#include <vector>
#include <string>

#include "AccessAttempt.h"
#include "Requester.h"

/**
 * @brief Records who tried to view which account, and what they were given.
 *
 * One log is shared by every AccountProxy, so all attempts end up in one place.
 */
class AccessLog
{
    //members
    private:
        /// @brief Every attempt, in the order it happened.
        std::vector<AccessAttempt> attempts;

    //functions
    public:
        /**
        * @brief Adds an attempt to the log.
        * @param r              Who asked.
        * @param accountNumber  The account they asked for.
        * @param level          The access level that was decided.
        * @note The timestamp is added automatically.
        */
        void record(const Requester& r, const std::string& accountNumber, AccessLevel level);

        /**
       * @brief Gets every recorded attempt.
        * @return A reference to the attempts; valid while this log exists.
        */
        const std::vector<AccessAttempt>& getAttempts() const;

        /**
        * @brief Gets only the attempts that were denied.
        * @return A new vector holding the denied attempts.
        */
        std::vector<AccessAttempt> getDenied() const;
};

#endif