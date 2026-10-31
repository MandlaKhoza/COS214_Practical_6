#include "DocumentBuilder.h"

void DocumentBuilder::populateTUI(std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields){
    //Will be implemented when the TUI is fully functional
}
void DocumentBuilder::createMapComponents(std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields){
    std::map<std::string, std::string> common;
    std::map<std::string, double> comparable;

    for(int i = 0; i < commonFields.size(); i++){
        common[commonFields[i]];
    }
    for (int j = 0; j < comparableFields.size(); j++){
        comparable[comparableFields[j]->getKey()];
    }

    this->document->setDocument(common, comparable);
}

void DocumentBuilder::setMapValues() {
    //Pulls keys from Document instance
    std::map<std::string, std::string> common = this->document->getCommon();
    std::map<std::string, double> comparable = this-> document->getComparable();
    //This will pass in the values from the TUI
}

Document* DocumentBuilder::getDocument() {
    return this->document;
}
