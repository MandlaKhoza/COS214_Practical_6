#include "tui_app.hpp"
#include "ui_logger.hpp"
#include "approval_prompt.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <atomic>
#include <algorithm>
#include <exception>
#include <string>
#include <thread>
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

void TUI::setValues(const std::map<std::string, std::string>& values) {
    for (const auto& entry : values) {
        if (common_values_.count(entry.first)) {
            common_values_[entry.first] = entry.second;
        } else if (comparable_values_.count(entry.first)) {
            comparable_values_[entry.first] = entry.second;
        }
    }
}

bool TUI::hasFields() const {
    return !common_names_.empty() || !comparable_names_.empty();
}

namespace {

ftxui::Element render_log_line(const std::string& log) {
    using namespace ftxui;
    struct PrefixColor {
        const char* prefix;
        Color color;
    };
    const PrefixColor table[] = {
        {"[DOC]", Color::Cyan},
        {"[RANGE]", Color::Yellow},
        {"[EXTERNAL]", Color::Magenta},
        {"[APPROVAL]", Color::Green},
        {"[WF]", Color::Blue},
        {"[TUI]", Color::White},
        {"[PASS]", Color::Green},
        {"[FAIL]", Color::Red},
        {"[NOTIFY]", Color::GrayLight}
    };
    const std::size_t count = sizeof(table) / sizeof(table[0]);
    for (std::size_t i = 0; i < count; ++i) {
        const std::string prefix = table[i].prefix;
        if (log.compare(0, prefix.size(), prefix) == 0) {
            return hbox({
                text(prefix) | bold | color(table[i].color),
                text(log.substr(prefix.size()))
            });
        }
    }
    return text(log);
}

ftxui::Elements render_logs(bool last_eight) {
    using namespace ftxui;
    Elements log_elements;
    const std::vector<std::string> logs = UILogger::instance().get_logs();
    std::size_t start = 0;
    if (last_eight && logs.size() > 8) {
        start = logs.size() - 8;
    }
    for (std::size_t i = start; i < logs.size(); ++i) {
        log_elements.push_back(render_log_line(logs[i]));
    }
    if (log_elements.empty()) {
        log_elements.push_back(text("Waiting for a document...") | dim);
    }
    return log_elements;
}

} // namespace

void TUI::run(std::function<void()> on_run_workflow, const std::string& title) {
    using namespace ftxui;

    auto screen = ScreenInteractive::Fullscreen();
    UILogger::instance().set_screen(&screen);

    auto labeled = [](const std::string& label, Component input) {
        return Renderer(input, [label, input] {
            return hbox({
                text(label) | size(WIDTH, EQUAL, 22) | color(Color::Cyan),
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

    std::thread worker;
    std::atomic<bool> running(false);

    auto join_worker = [&worker]() {
        if (worker.joinable()) {
            worker.join();
        }
    };

    auto run_button = Button(" Run Workflow ", [&] {
        if (running.exchange(true)) {
            UI_LOG("[TUI] Workflow already running.");
            return;
        }
        join_worker();
        UILogger::instance().clear();
        UI_LOG("[TUI] Run Workflow clicked — copying TUI values into the Document.");
        worker = std::thread([&] {
            try {
                if (on_run_workflow) {
                    on_run_workflow();
                }
            } catch (const std::exception& e) {
                UI_LOG(std::string("[WF] Exception: ") + e.what());
            }
            running = false;
            UILogger::instance().request_redraw();
        });
    });
    auto quit_button = Button(" Close Scenario ", [&] {
        ApprovalPrompt::instance().answer(false);
        screen.Exit();
    });
    form_children.push_back(Container::Horizontal({ run_button, quit_button }));

    auto form = Container::Vertical(std::move(form_children));

    auto pane_doc = Renderer(form, [&] {
        Elements body;
        body.push_back(text(" Document Details ") | bold | color(Color::Blue));
        body.push_back(separator());
        if (!hasFields()) {
            body.push_back(text("No fields registered. DocumentBuilder::populateTUI was not called.") | dim);
        } else {
            body.push_back(text("Fields created by DocumentBuilder. Fill them, then Run Workflow.") | dim);
            body.push_back(text("Demo values are prefilled and editable. Close Scenario to continue or exit.") | dim);
            body.push_back(separator());
            body.push_back(form->Render() | flex);
        }
        return vbox(std::move(body)) | border | flex;
    });

    auto tab1 = Renderer(pane_doc, [&] {
        return vbox({
            pane_doc->Render() | flex,
            vbox({
                text(" Live Logs ") | bold | bgcolor(Color::Blue),
                separator(),
                vbox(render_logs(true)) | yframe | vscroll_indicator | size(HEIGHT, EQUAL, 8)
            }) | border
        });
    });

    // Console scrolling
    std::vector<std::string> console_logs;
    int console_selected = 0;
    bool console_initialized = false;

    MenuOption option;

    option.entries_option.transform = [](const EntryState& state) {
        Element entry = render_log_line(state.label);

        if (state.focused) {
            entry = entry | inverted;
        }

        return entry;
    };

    auto console_menu = Menu(&console_logs, &console_selected, option);

    auto tab2 = Renderer(console_menu, [&] {
        console_logs = UILogger::instance().get_logs();

        if (!console_initialized && !console_logs.empty()) {
            console_selected = static_cast<int>(console_logs.size()) - 1;
            console_initialized = true;
        }

        if (console_logs.empty()) {
            console_selected = 0;
        } else if (console_selected >= static_cast<int>(console_logs.size())) {
            console_selected = static_cast<int>(console_logs.size()) - 1;
        }

        return vbox({
            text(" System Output Console ") | bold | bgcolor(Color::Blue),
            separator(),
            console_menu->Render() | vscroll_indicator | frame | flex
        }) | border | flex;
    });

    int tab_index = 0;
    std::vector<std::string> tab_labels;
    tab_labels.push_back("Document Builder & Controls");
    tab_labels.push_back("System Output Console");
    auto tab_toggle = Toggle(&tab_labels, &tab_index);
    auto tab_content = Container::Tab({ tab1, tab2 }, &tab_index);

    auto layout = Container::Vertical({ tab_toggle, tab_content });
    auto renderer = Renderer(layout, [&] {
        return vbox({
            hbox({
                text(" TIVIDY ") | bold | bgcolor(Color::Cyan) | color(Color::Black),
                separator(),
                text(" " + title + " ") | bold,
                separator(),
                tab_toggle->Render() | flex
            }),
            separator(),
            tab_content->Render() | flex
        });
    });

    bool show_popup = false;
    ApprovalPrompt::instance().setNotifier([&] {
        screen.Post([&] { show_popup = true; });
    });
    auto yes_button = Button(" Yes ", [&] {
        UI_LOG("[APPROVAL] Human selected Yes.");
        ApprovalPrompt::instance().answer(true);
        show_popup = false;
    });
    auto no_button = Button(" No ", [&] {
        UI_LOG("[APPROVAL] Human selected No.");
        ApprovalPrompt::instance().answer(false);
        show_popup = false;
    });
    auto modal_buttons = Container::Horizontal({ yes_button, no_button });
    auto modal = Renderer(modal_buttons, [&] {
        return vbox({
            text(" Approval required ") | bold | color(Color::Yellow),
            separator(),
            text(ApprovalPrompt::instance().message()),
            separator(),
            text("Approve this step and continue the workflow?") | dim,
            separator(),
            modal_buttons->Render() | center
        }) | border | color(Color::Yellow) | size(WIDTH, GREATER_THAN, 40);
    });

    auto app = CatchEvent(renderer, [&](Event) {
        if (ApprovalPrompt::instance().isPending()) {
            show_popup = true;
        }
        return false;
    });
    app |= Modal(modal, &show_popup);

    UI_LOG("[TUI] Document section populated from commonFields and comparableFields.");
    screen.Loop(app);

    ApprovalPrompt::instance().setNotifier(nullptr);
    ApprovalPrompt::instance().answer(false);
    join_worker();
    UILogger::instance().set_screen(nullptr);
}
