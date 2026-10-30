#ifndef EMPLOYEEACCOUNT_H
#define EMPLOYEEACCOUNT_H

#include "Account.h"

class EmployeeAccount : public Account{
private:
    std::string employeeNumber;
    std::string department;
    std::string systemAccess;

public:
EmployeeAccount(std::string number, std::string type, std::string ownerId, std::string createdAt,
    std::string employeeNumber, std::string department, std::string systemAccess);
    std::string describe() const;
    std::map<std::string, std::string> getDetails() const override;
};

#endif 