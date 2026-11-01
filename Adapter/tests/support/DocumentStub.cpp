/**
 * @file DocumentStub.cpp
 * @brief TEMPORARY test stand-in for Document, so the adapter tests can run before the
 *        Builder owner pushes the real src/.../Document.cpp.
 *
 * Only the adapter test build uses this file. Delete it (and point the test build at the
 * real Document.cpp) once that exists. Never commit it over the real implementation.
 */
#include "Document.h"

#include <sstream>

Document::Document() {}

void Document::setType(const std::string& type) { workflowType = type; }
const std::string& Document::getType() const { return workflowType; }

void Document::addTextField(const std::string& key) { common[key] = ""; }
void Document::addNumberField(const std::string& key) { comparable[key] = 0.0; }
void Document::markOptional(const std::string& key) { optional.insert(key); }

bool Document::setText(const std::string& key, const std::string& value) {
    if (common.find(key) == common.end()) return false;
    common[key] = value;
    filled.insert(key);
    return true;
}

bool Document::setNumber(const std::string& key, double value) {
    if (comparable.find(key) == comparable.end()) return false;
    comparable[key] = value;
    filled.insert(key);
    return true;
}

bool Document::getField(const std::string& key, std::string& out) const {
    if (!isFilled(key)) return false;
    std::map<std::string, std::string>::const_iterator t = common.find(key);
    if (t != common.end()) { out = t->second; return true; }
    std::map<std::string, double>::const_iterator n = comparable.find(key);
    if (n != comparable.end()) { std::ostringstream s; s << n->second; out = s.str(); return true; }
    return false;
}

bool Document::getNumber(const std::string& key, double& out) const {
    std::map<std::string, double>::const_iterator n = comparable.find(key);
    if (n == comparable.end() || !isFilled(key)) return false;
    out = n->second;
    return true;
}

bool Document::isFilled(const std::string& key) const { return filled.count(key) > 0; }

std::vector<std::string> Document::missingRequiredFields() const {
    std::vector<std::string> missing;
    for (std::map<std::string, std::string>::const_iterator i = common.begin(); i != common.end(); ++i)
        if (!isFilled(i->first) && !optional.count(i->first)) missing.push_back(i->first);
    for (std::map<std::string, double>::const_iterator i = comparable.begin(); i != comparable.end(); ++i)
        if (!isFilled(i->first) && !optional.count(i->first)) missing.push_back(i->first);
    return missing;
}

bool Document::isComplete() const { return missingRequiredFields().empty(); }
const std::map<std::string, std::string>& Document::getCommon() const { return common; }
const std::map<std::string, double>& Document::getComparable() const { return comparable; }
