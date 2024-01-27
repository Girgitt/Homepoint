#pragma once

#include <memory>
#include <mutex>
#include <cmath>

extern "C" 
{
  #include "esp_log.h"
}

namespace sgs 
{
    static const char *LOG_TAG = "SharedGlobalState";

    class SharedGlobalState {
    public:
        SharedGlobalState() : uptime_sec(0), last_log_print_sec(0), uptime_ms(0), last_tap_ms(0), sec_since_last_tap(0) {}
        
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
            
            // in case of uptime_ms overflow every 49.7 days the idle time is truncated to max 24h
            if (last_tap_ms > uptime_ms)
            {
                ESP_LOGI(LOG_TAG, "uptime_ms overflow"); 
                last_tap_ms = 0;
                if (sec_since_last_tap < 24 * 3600)
                {
                    uptime_ms = sec_since_last_tap * 1000;
                }
                else{
                    uptime_ms = 24 * 3600 * 1000;
                }
            }
            sec_since_last_tap = static_cast<int>(std::floor((uptime_ms - last_tap_ms) / 1000.0));

            if (uptime_sec % 5 == 0){
                if(uptime_sec != last_log_print_sec){
                    ESP_LOGI(LOG_TAG, "uptime %d sec", uptime_sec);        
                    ESP_LOGI(LOG_TAG, "sec_since_last_tap  %d sec", sec_since_last_tap);
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

        int getIdleTimeSec() const {
            std::lock_guard<std::mutex> lock(mutex);
            return sec_since_last_tap;
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
        int sec_since_last_tap;
        mutable std::mutex mutex;   // Note: mutable is used to allow locking in const member functions
    };

    // Define the shared instance
    extern SharedGlobalState& sharedGlobalState;

} // namespace sgs