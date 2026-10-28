#include "Director.h"

Director::Director(Builder* builder, std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields) {
    this->builder = builder;
    this->commonFields = commonFields;
    this->comparableFields = comparableFields;
}

Document* Director::construct() {
    this->builder->populateTUI(this->commonFields, this->comparableFields);
    this->builder->createMapComponents(this->commonFields, this->comparableFields);
    this->builder->setMapValues();

    return this->builder->getDocument();
}
