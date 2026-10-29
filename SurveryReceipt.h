#ifndef SURVEYRECEIPT_H
#define SURVEYRECEIPT_H

#include "Account.h"

class SurveyReceipt : public Account{
private:
    std::string responseId;
    double summaryScore;

public:
    std::string describe() const override;
    std::map<std::string, std::string> getDetails() const override;
};

#endif 