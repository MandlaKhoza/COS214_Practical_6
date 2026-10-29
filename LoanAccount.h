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
    std::string describe() const override;
    std::map<std::string, std::string> getDetails() const override;
};

#endif 