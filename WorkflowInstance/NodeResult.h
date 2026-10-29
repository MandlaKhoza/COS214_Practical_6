#ifndef NODERESULT_H
#define NODERESULT_H

#include <string>

enum Result {
    PASSED,
    REJECTED,
    PENDING,
    FAILED,
    CANCELLED
};

struct NodeResult {
        Result result;
        std::string name;
};

#endif
