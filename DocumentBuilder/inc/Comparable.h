#include <string>
#include <iostream>

class Comparable {
    private:
        std::string key;
        double conditionValue;
        std::string operand;

    public:
        Comparable(std::string key, double conditionValue, std::string operand) {
            this->key = key;
            this->conditionValue = conditionValue;
            this->operand = operand;
        }

        bool compare (double value) {
            if (this->operand == "=") {
                if (value == conditionValue) return true;
                else return false;
            } else if (this->operand == "!=") {
                if (value != conditionValue) return true;
                else return false;
            } else if (this->operand == ">") {
                if (value > conditionValue) return true;
                else return false;
            } else if (this->operand == ">=") {
                if (value >= conditionValue) return true;
                else return false;
            } else if (this->operand == "<") {
                if (value < conditionValue) return true;
                else return false;
            } else if (this->operand == "<=") {
                if (value <= conditionValue) return true;
                else return false;
            }

            std::cout << "Invalid operand for Comparable" << std::endl;
            return false;
        }

        std::string getKey() {return this->key;}
};
