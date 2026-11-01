#ifndef LOANACCOUNT_H
#define LOANACCOUNT_H

#include "Account.h"

class LoanAccount : public Account{
private:
    double principal;
    double interestRate;
    int termMonths;
    double monthlyInstalment;
    double outstanding;

public:
LoanAccount(std::string number, std::string type, std::string ownerId, std::string createdAt,
    double principal, double interestRate, int termMonths, double monthlyInstalment, double outstanding);
    std::string describe() const;
    std::map<std::string, std::string> getDetails() const override;
};

#endif 