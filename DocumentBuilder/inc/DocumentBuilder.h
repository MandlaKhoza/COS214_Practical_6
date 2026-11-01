#ifndef DOCUMENTBUILDER_H
#define DOCUMENTBUILDER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Builder.h"
#include "Document.h"

class TUI;
class DocumentBuilder : public Builder {
private:
    Document* document;
    TUI* stage;
public:
    DocumentBuilder(Document* document, TUI* stage);
    void populateTUI(std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields) override;
    void createMapComponents(std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields) override;
    void setMapValues() override;
    Document *getDocument() override;
};

#endif // DOCUMENTBUILDER_H
