#include "tui_app.hpp"
#include "ui_logger.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <string>
#include <vector>

void TUI::addCommonField(const std::string& name) {
    if (common_values_.count(name)) {
        return;
    }
    common_names_.push_back(name);
    common_values_[name] = "";
}

void TUI::addComparableField(const std::string& name) {
    if (comparable_values_.count(name)) {
        return;
    }
    comparable_names_.push_back(name);
    comparable_values_[name] = "";
}

std::string TUI::getCommonValue(const std::string& name) const {
    auto it = common_values_.find(name);
    return it == common_values_.end() ? "" : it->second;
}

std::string TUI::getComparableValue(const std::string& name) const {
    auto it = comparable_values_.find(name);
    return it == comparable_values_.end() ? "" : it->second;
}

bool TUI::hasFields() const {
    return !common_names_.empty() || !comparable_names_.empty();
}

void TUI::run(std::function<void()> on_run_workflow) {
    using namespace ftxui;

    auto screen = ScreenInteractive::Fullscreen();
    UILogger::instance().set_screen(&screen);

    auto labeled = [](const std::string& label, Component input) {
        return Renderer(input, [label, input] {
            return hbox({
                text(label) | size(WIDTH, EQUAL, 12) | color(Color::Cyan),
                text(" "),
                input->Render() | flex
            });
        });
    };

    Components form_children;
    for (const auto& name : common_names_) {
        form_children.push_back(labeled(name, Input(&common_values_[name], name)));
    }
    for (const auto& name : comparable_names_) {
        form_children.push_back(labeled(name, Input(&comparable_values_[name], "number")));
    }

    auto run_button = Button(" Run workflow ", [&] {
        UI_LOG("Run workflow clicked — copying TUI values into the Document.");
        if (on_run_workflow) {
            on_run_workflow();
        }
    });
    auto quit_button = Button(" Quit ", screen.ExitLoopClosure());
    form_children.push_back(Container::Horizontal({ run_button, quit_button }));

    auto form = Container::Vertical(std::move(form_children));

    auto pane_doc = Renderer(form, [&] {
        Elements body;
        body.push_back(text(" Document Details ") | bold | color(Color::Blue));
        body.push_back(separator());
        if (!hasFields()) {
            body.push_back(text("No fields registered. DocumentBuilder::populateTUI was not called.") | dim);
        } else {
            body.push_back(text("Fields created by DocumentBuilder. Fill them, then Run workflow.") | dim);
            body.push_back(separator());
            body.push_back(form->Render() | flex);
        }
        return vbox(std::move(body)) | border | flex;
    });

    auto pane_logs = Renderer([&] {
        Elements log_elements;
        for (const auto& log : UILogger::instance().get_logs()) {
            log_elements.push_back(text(log));
        }
        if (log_elements.empty()) {
            log_elements.push_back(text("Waiting for a document...") | dim);
        }
        return vbox({
            text(" Execution Console ") | bold | bgcolor(Color::Blue),
            separator(),
            vbox(std::move(log_elements)) | yframe | flex
        }) | border | flex;
    });

    auto layout = Container::Vertical({ pane_doc, pane_logs });
    auto renderer = Renderer(layout, [&] {
        return vbox({
            hbox({
                text(" DEMO SUITE ") | bold | bgcolor(Color::Cyan) | color(Color::Black),
                separator(),
                text(" Tab between fields. Run workflow copies values into the Document.") | dim
            }),
            separator(),
            pane_doc->Render() | flex,
            pane_logs->Render() | flex
        });
    });

    UI_LOG("Document section populated from commonFields and comparableFields.");
    screen.Loop(renderer);
}
