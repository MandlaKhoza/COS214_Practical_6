#ifndef EMPLOYEEACCOUNT_H
#define EMPLOYEEACCOUNT_H

#include "Account.h"

class EmployeeAccount : public Account{
private:
    std::string employeeNumber;
    std::string department;
    std::string systemAccess;

public:
    std::string describe() const override;
    std::map<std::string, std::string> getDetails() const override;
};

#endif 