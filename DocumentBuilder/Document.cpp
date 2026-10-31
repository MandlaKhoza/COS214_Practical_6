#include "Document.h"

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
