#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <map>
#include <set>
#include <string>
#include <vector>

/**
 * @brief Builder Product: the applicant's filled-in form.
 *
 * Text and number fields are kept in two maps. A double can't be blank,
 * so `filled` records which keys the user actually answered.
 */
class Document {
public:
    Document();

    void setType(const std::string& type);
    const std::string& getType() const;

    void addTextField(const std::string& key);    ///< key with an empty value
    void addNumberField(const std::string& key);  ///< key with 0, not filled
    void markOptional(const std::string& key);

    /// @return false if the form has no such text field; marks the key filled.
    bool setText(const std::string& key, const std::string& value);
    /// @return false if the form has no such number field; marks the key filled.
    bool setNumber(const std::string& key, double value);

    /// The single lookup for adapters: checks text, then numbers (converted to text).
    /// @return false if missing or not filled.
    bool getField(const std::string& key, std::string& out) const;
    /// @return false if missing, not a number field, or not filled.
    bool getNumber(const std::string& key, double& out) const;
    bool isFilled(const std::string& key) const;

    /// Fields that are not filled and not optional (used by the completeness node).
    std::vector<std::string> missingRequiredFields() const;
    bool isComplete() const;

    const std::map<std::string, std::string>& getCommon() const;
    const std::map<std::string, double>& getComparable() const;

private:
    std::string workflowType;
    std::map<std::string, std::string> common;
    std::map<std::string, double> comparable;
    std::set<std::string> filled;
    std::set<std::string> optional;
};

#endif
