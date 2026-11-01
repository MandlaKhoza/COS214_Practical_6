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

    // Write a log line (Thread-Safe)
    void log(const std::string& msg) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            logs_.push_back(msg);
            
            // Ring Buffer Cap: Keep memory usage low
            if (logs_.size() > 100) {
                logs_.erase(logs_.begin());
            }
        }

        // Tell FTXUI to redraw immediately
        if (screen_) {
            screen_->PostEvent(ftxui::Event::Custom);
        }
    }

    // Retrieve logs for rendering (Thread-Safe)
    std::vector<std::string> get_logs() {
        std::lock_guard<std::mutex> lock(mutex_);
        return logs_;
    }

private:
    UILogger() = default; // Private constructor prevents duplicate instances
    std::mutex mutex_;
    std::vector<std::string> logs_;
    ftxui::ScreenInteractive* screen_ = nullptr;
};

// Global helper function for your team
inline void UI_LOG(const std::string& msg) {
    UILogger::instance().log(msg);
}