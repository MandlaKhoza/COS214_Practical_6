#pragma once

#include "ui_logger.hpp"

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>

// Blocks a workflow worker until the TUI modal records Yes/No.
// When no FTXUI screen is attached (unit tests / headless), waitForDecision
// returns true so existing ApprovalChain tests keep running.
class ApprovalPrompt {
public:
    static ApprovalPrompt& instance() {
        static ApprovalPrompt inst;
        return inst;
    }

    bool waitForDecision(const std::string& message) {
        if (!UILogger::instance().has_screen()) {
            return true;
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            message_ = message;
            pending_ = true;
            answered_ = false;
            decision_ = false;
        }

        if (notifier_) {
            notifier_();
        }
        UILogger::instance().request_redraw();

        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return answered_; });
        pending_ = false;
        return decision_;
    }

    bool isPending() {
        std::lock_guard<std::mutex> lock(mutex_);
        return pending_ && !answered_;
    }

    std::string message() {
        std::lock_guard<std::mutex> lock(mutex_);
        return message_;
    }

    void setNotifier(std::function<void()> notifier) {
        notifier_ = std::move(notifier);
    }

    void answer(bool yes) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!pending_ || answered_) {
                return;
            }
            decision_ = yes;
            answered_ = true;
            pending_ = false;
        }
        cv_.notify_all();
    }

private:
    ApprovalPrompt() = default;

    std::mutex mutex_;
    std::condition_variable cv_;
    std::string message_;
    bool pending_ = false;
    bool answered_ = false;
    bool decision_ = false;
    std::function<void()> notifier_;
};
