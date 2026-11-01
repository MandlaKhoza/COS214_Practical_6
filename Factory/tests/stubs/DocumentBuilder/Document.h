#ifndef TEST_DOCUMENT_H
#define TEST_DOCUMENT_H

#include <string>
#include <map>

class Document {
private:
    std::map<std::string, std::string> fields;
    std::map<std::string, double> numbers;

public:
    void setField(const std::string& name, const std::string& value) {
        fields[name] = value;
    }

    void setNumber(const std::string& name, double value) {
        numbers[name] = value;
    }

    bool getField(const std::string& name, std::string& value) const {
        std::map<std::string, std::string>::const_iterator it = fields.find(name);

        if (it == fields.end()) {
            return false;
        }

        value = it->second;
        return true;
    }

    bool getNumber(const std::string& name, double& value) const {
        std::map<std::string, double>::const_iterator it = numbers.find(name);

        if (it == numbers.end()) {
            return false;
        }

        value = it->second;
        return true;
    }
};

#endif
