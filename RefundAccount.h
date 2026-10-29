#ifndef REFUNDRECORD_H
#define REFUNDRECORD_H

#include "Account.h"

class RefundRecord : public Account {
private:
    std::string orderNumber;
    double amount;
    std::string payoutMethod;

public:
    std::string describe() const override;
    std::map<std::string, std::string> getDetails() const override;
};

#endif