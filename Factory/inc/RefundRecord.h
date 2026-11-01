#ifndef REFUNDRECORD_H
#define REFUNDRECORD_H

#include "Account.h"

class RefundRecord : public Account{
private:
    std::string orderNumber;
    double amount;
    std::string payoutMethod;

public:
RefundRecord(std::string number, std::string type, std::string ownerId, std::string createdAt,
    std::string orderNumber, double amount, std::string payoutMethod);
    std::string describe() const ;
    std::map<std::string, std::string> getDetails() const override;
};

#endif