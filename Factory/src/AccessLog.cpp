#include <ctime>

#include "AccessLog.h"

void AccessLog::record(const Requester& r, const std::string& accountNumber, AccessLevel level)
{
    std::time_t now = std::time(nullptr);
    std::tm* parts = std::localtime(&now);

    if (parts == nullptr)
    {
        return;
    }

    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", parts);

    std::string ts = std::string(buffer);

    AccessAttempt aA = AccessAttempt(ts, r.userId, accountNumber, level);
    attempts.push_back(aA);
}

const std::vector<AccessAttempt>& AccessLog::getAttempts() const
{
    return attempts;
}

std::vector<AccessAttempt> AccessLog::getDenied() const
{
    std::vector<AccessAttempt> aL;
    for (std::size_t i = 0; i < attempts.size(); i++)
    {
        if (attempts[i].level == AccessLevel::Denied)
        {
            aL.push_back(attempts[i]);
        }
    }
    return aL;
}