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

    // Builds the form from fields registered by DocumentBuilder::populateTUI.
    // on_run_workflow is invoked on a background thread after "Run Workflow".
    void run(std::function<void()> on_run_workflow);

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
