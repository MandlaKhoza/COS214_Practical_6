#ifndef TEST_WORKFLOW_INSTANCE_H
#define TEST_WORKFLOW_INSTANCE_H

#include <string>
#include <memory>
#include "DocumentBuilder/Document.h"

class WorkflowInstance {
private:
    std::shared_ptr<Document> document;
    std::string type;

public:
    WorkflowInstance(std::shared_ptr<Document> document,
                     const std::string& type)
        : document(document), type(type) {}

    std::shared_ptr<Document> getDocument() const {
        return document;
    }

    const std::string& getType() const {
        return type;
    }
};

#endif
