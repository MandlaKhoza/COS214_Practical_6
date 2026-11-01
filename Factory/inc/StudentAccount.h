#ifndef STUDENTACCOUNT_H
#define STUDENTACCOUNT_H

#include "Account.h"

class StudentAccount : public Account{
private:
    std::string studentNumber;
    std::string faculty;
    std::string programme;

public:
StudentAccount(std::string number, std::string type, std::string ownerId, std::string createdAt,
    std::string studentNumber, std::string faculty, std::string programme);
    
    std::string describe() const;
    std::map<std::string, std::string> getDetails() const override;
};

#endif 