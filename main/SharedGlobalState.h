#pragma once

#include <memory>
#include <mutex>
#include <cmath>

extern "C" 
{
  #include "esp_log.h"
}

namespace sgs {

class SharedGlobalState {
public:
    SharedGlobalState() : uptime_sec(0), last_log_print_sec(0), uptime_ms(0), last_tap_ms(0) {}
    
    // when using with C-compatible functions version
    static SharedGlobalState& getInstance() {
        static SharedGlobalState instance;
        return instance;
    }
    //static std::shared_ptr<SharedGlobalState> getInstance();

    void tickUptimeMs(int ms){
        std::lock_guard<std::mutex> lock(mutex);
        uptime_ms += static_cast<long>(ms);
        uptime_sec = std::floor(static_cast<double>(uptime_ms)/1000.0);
        if (uptime_sec % 5 == 0){
            if(uptime_sec != last_log_print_sec){
                ESP_LOGI("SharedGlobalState", "uptime %d sec", uptime_sec);
                long sec_since_last_tap = std::floor((uptime_ms - last_tap_ms) / 1000.0);
                ESP_LOGI("SharedGlobalState", "sec_since_last_tap  %d sec", static_cast<int>(sec_since_last_tap));
                last_log_print_sec = uptime_sec;
            }
        }

    }
    void setUptime(int new_uptime) {
        std::lock_guard<std::mutex> lock(mutex);
        uptime_sec = new_uptime;
    }

    int getUptime() const {
        std::lock_guard<std::mutex> lock(mutex);
        return uptime_sec;
    }
    
    void registerTap(){
        std::lock_guard<std::mutex> lock(mutex);
        
        ESP_LOGI("SharedGlobalState", "registering tap");

        last_tap_ms = uptime_ms;
    }

private:
    int uptime_sec;
    int last_log_print_sec;
    long uptime_ms;
    long last_tap_ms;
    mutable std::mutex mutex;   // Note: mutable is used to allow locking in const member functions
};

// Define the shared instance
extern SharedGlobalState& sharedGlobalState;

} // namespace sgs