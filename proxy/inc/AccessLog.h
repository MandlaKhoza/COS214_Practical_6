#ifndef ACCESSLOG_H
#define ACCESSLOG_H

#include <vector>

#include "AccessAttempt.h"
#include "Requester.h"

class AccessLog
{
    //members
    private:
        std::vector<AccessAttempt> attempts;

    //functions
    public:
        void record(const Requester& r, const std::string& accountNumber, AccessLevel level);
        const std::vector<AccessAttempt>& getAttempts() const;
        std::vector<AccessAttempt> getDenied() const;
};

#endif