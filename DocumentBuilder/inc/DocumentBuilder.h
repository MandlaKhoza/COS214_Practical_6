#ifndef DOCUMENTBUILDER_H
#define DOCUMENTBUILDER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Builder.h"
#include "Document.h"

class TUI; // Anti-Errorinator || Empty Promises || Dad leaving to buy some milk
class DocumentBuilder {
private:
    Document* document;
    TUI* stage;
public:
    DocumentBuilder(Document* document, TUI* stage);
    void populateTUI(std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields);
    void createMapComponents(std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields);
    void setMapValues();
    Document *getDocument();
};

#endif // BUILDER_H
