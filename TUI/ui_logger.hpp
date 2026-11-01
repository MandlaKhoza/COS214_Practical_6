#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <ftxui/component/screen_interactive.hpp>

class UILogger {
public:
    // Global access point
    static UILogger& instance() {
        static UILogger inst;
        return inst;
    }

    // Connect FTXUI screen so the logger can trigger UI updates
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

    // Write a log line (Thread-Safe)
    void log(const std::string& msg) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            logs_.push_back(msg);

            // Ring Buffer Cap: Keep memory usage low
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

    // Retrieve logs for rendering (Thread-Safe)
    std::vector<std::string> get_logs() {
        std::lock_guard<std::mutex> lock(mutex_);
        return logs_;
    }

private:
    UILogger() = default; // Private constructor prevents duplicate instances
    mutable std::mutex mutex_;
    std::vector<std::string> logs_;
    ftxui::ScreenInteractive* screen_ = nullptr;
};

// Global helper function for your team
inline void UI_LOG(const std::string& msg) {
    UILogger::instance().log(msg);
}
