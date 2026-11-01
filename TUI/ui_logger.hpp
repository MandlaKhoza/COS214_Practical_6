#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <ftxui/component/screen_interactive.hpp>

class UILogger {
public:

    static UILogger& instance() {
        static UILogger inst;
        return inst;
    }


    void set_screen(ftxui::ScreenInteractive* screen) {
        std::lock_guard<std::mutex> lock(mutex_);
        screen_ = screen;
    }

    bool has_screen() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return screen_ != nullptr;
    }

    void request_redraw() {
        ftxui::ScreenInteractive* screen = nullptr;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            screen = screen_;
        }
        if (screen) {
            screen->PostEvent(ftxui::Event::Custom);
        }
    }


    void log(const std::string& msg) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            logs_.push_back(msg);


            if (logs_.size() > 200) {
                logs_.erase(logs_.begin());
            }
        }

        request_redraw();
    }

    void clear() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            logs_.clear();
        }
        request_redraw();
    }


    std::vector<std::string> get_logs() {
        std::lock_guard<std::mutex> lock(mutex_);
        return logs_;
    }

private:
    UILogger() = default;
    mutable std::mutex mutex_;
    std::vector<std::string> logs_;
    ftxui::ScreenInteractive* screen_ = nullptr;
};


inline void UI_LOG(const std::string& msg) {
    UILogger::instance().log(msg);
}
