#include "../inc/DocumentBuilder.h"
#include "tui_app.hpp"

#include <cmath>
#include <limits>
#include <string>

DocumentBuilder::DocumentBuilder(Document* document, TUI* stage)
    : document(document), stage(stage) {}

void DocumentBuilder::populateTUI(std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields){
    if (!stage) {
        return;
    }

    for (std::size_t i = 0; i < commonFields.size(); ++i) {
        stage->addCommonField(commonFields[i]);
    }
    for (std::size_t j = 0; j < comparableFields.size(); ++j) {
        if (comparableFields[j]) {
            stage->addComparableField(comparableFields[j]->getKey());
        }
    }
}

void DocumentBuilder::createMapComponents(std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields){
    std::map<std::string, std::string> common;
    std::map<std::string, double> comparable;

    for(int i = 0; i < static_cast<int>(commonFields.size()); i++){
        common[commonFields[i]] = "";
    }
    for (int j = 0; j < static_cast<int>(comparableFields.size()); j++){
        if (comparableFields[j]) {
            comparable[comparableFields[j]->getKey()] =
                std::numeric_limits<double>::quiet_NaN();
        }
    }

    this->document->setDocument(common, comparable);
}

void DocumentBuilder::setMapValues() {
    if (!document || !stage) {
        return;
    }

    std::map<std::string, std::string> common = this->document->getCommon();
    std::map<std::string, double> comparable = this->document->getComparable();

    for (std::map<std::string, std::string>::iterator it = common.begin();
         it != common.end(); ++it) {
        it->second = stage->getCommonValue(it->first);
    }

    for (std::map<std::string, double>::iterator it = comparable.begin();
         it != comparable.end(); ++it) {
        const std::string raw = stage->getComparableValue(it->first);
        if (raw.empty()) {
            it->second = std::numeric_limits<double>::quiet_NaN();
            continue;
        }
        try {
            std::size_t idx = 0;
            const double parsed = std::stod(raw, &idx);
            if (idx == 0) {
                it->second = std::numeric_limits<double>::quiet_NaN();
            } else {
                it->second = parsed;
            }
        } catch (...) {
            it->second = std::numeric_limits<double>::quiet_NaN();
        }
    }

    this->document->setDocument(common, comparable);
}

Document* DocumentBuilder::getDocument() {
    return this->document;
}
