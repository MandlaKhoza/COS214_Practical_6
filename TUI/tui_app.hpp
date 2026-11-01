#pragma once
#include <functional>
#include <map>
#include <string>
#include <vector>

class TUI {
public:
    void addCommonField(const std::string& name);
    void addComparableField(const std::string& name);

    std::string getCommonValue(const std::string& name) const;
    std::string getComparableValue(const std::string& name) const;

    bool hasFields() const;
    void setValues(const std::map<std::string, std::string>& values);


    void run(std::function<void()> on_run_workflow,
             const std::string& title = "Workflow");

private:
    std::vector<std::string> common_names_;
    std::vector<std::string> comparable_names_;
    std::map<std::string, std::string> common_values_;
    std::map<std::string, std::string> comparable_values_;
};

class TUIApp {
public:
    static int run();
};
