#ifndef STUDENTACCOUNT_H
#define STUDENTACCOUNT_H

#include "Account.h"

class StudentAccount : public Account{
private:
    std::string studentNumber;
    std::string faculty;
    std::string programme;

public:
    std::string describe() const override;
    std::map<std::string, std::string> getDetails() const override;
};

#endif 