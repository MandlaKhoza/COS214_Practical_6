#ifndef SURVEYRECEIPT_H
#define SURVEYRECEIPT_H

#include "Account.h"

class SurveyReceipt : public Account{
private:
    std::string responseId;
    double summaryScore;

public:
SurveyReceipt(std::string number, std::string type, std::string ownerId, std::string createdAt,
     std::string responseId, double summaryScore);
    std::string describe() const;
    std::map<std::string, std::string> getDetails() const override;
};

#endif 