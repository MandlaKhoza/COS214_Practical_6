#ifndef NODERESULT_H
#define NODERESULT_H

#include <string>

enum HandleResult {
    PASSED,
    REJECTED,
    PENDING,
    FAILED,
    CANCELLED
};

struct NodeResult {
        HandleResult result;
        std::string name;
};

#endif
