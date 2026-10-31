#include "inc/Document.h"

void Document::setDocument(std::map<std::string, std::string> common, std::map<std::string, double> comparable) {
    this->common = common;
    this->comparable = comparable;
}

std::map<std::string, std::string> Document::getCommon() {
    return this->common;
}

std::map<std::string, double> Document::getComparable() {
    return this->comparable;
}


bool Document::getField(std::string fieldName, std::string& value) const{
    std::map<std::string, std::string>::const_iterator it;
    it = common.find(fieldName);
    if(it == common.end()){
        return false;
    }
    value = it->second;
    return true;
}

bool Document::getNumber(std::string fieldName, double& value) const{
    std::map<std::string, double>::const_iterator it;

    it = comparable.find(fieldName);
    if(it == comparable.end()){
        return false;
    }
    value = it->second;
    return true;
}