#pragma once

#include <AppContext.h>
#include <config/PlatformInject.hpp>
#include <touch/TouchTypes.hpp>

#include <chrono>
#include <memory>
#include <optional>

#include <SharedGlobalState.h>

#define LOG_TAG "screen saver"

namespace gfx
{
  template<typename ScreenDriver>
  struct ScreenSaver
  {
    public:
      ScreenSaver(ScreenDriver* driver, std::shared_ptr<ctx::AppContext> ctx) :
        mpDriver(driver),
        mpCtx(ctx)
      {
        // //initial touch time
        // mLastTouch = std::chrono::system_clock::now();
        power_save_freq_mhz = mpCtx->getModel().mHardwareConfig.mPowerSaveFreq;
        performance_freq_mhz = mpCtx->getModel().mHardwareConfig.mPerformanceFreq;
        power_save_enabled = mpCtx->getModel().mHardwareConfig.mIsScreenSaverPowerSaveEnabled;
      }

      bool operator()()
      {
        //auto now = std::chrono::system_clock::now();
        const auto timeOutMin = mpCtx->getModel().mHardwareConfig.mScreensaverMins;
        const auto idleSec = sgs::sharedGlobalState.getIdleTimeSec();
        const auto uptime_sec = sgs::sharedGlobalState.getUptime();
        if (uptime_sec % 5 == 0){
            if(uptime_sec != last_log_print_sec){
                ESP_LOGI(LOG_TAG,  "idleSec  %d sec", idleSec);
                ESP_LOGI(LOG_TAG,  "timeOutMin  %d min", timeOutMin);
                last_log_print_sec = uptime_sec;
            }
        }

        if (static_cast<int>(sgs::sharedGlobalState.getIdleTimeSec()) < static_cast<int>(timeOutMin) * 60 )
        {
          return switchScreen(screen_on);
        }
        else
        {
          return switchScreen(screen_off);
        }
      }

      // template<typename T>
      // bool tapped(const T& tapEvt)
      // {
      //   if (!tapEvt)
      //   {
      //     return mCurrentState;
      //   }
      //   auto tapEvent = *tapEvt;
      //   if (tapEvent.state == decltype(tapEvent.state)::Tap)
      //   {
      //     mLastTouch = std::chrono::system_clock::now();
      //   }
      //   return mCurrentState;
      // }

      // void activate()
      // {
      //   mLastTouch = std::chrono::system_clock::now();
      // }

    private:

      bool switchScreen(bool new_screen_state)
      {
        if (new_screen_state == mCurrentState)
        {
          return mCurrentState;
        }
        
        if (new_screen_state)
        {
          ESP_LOGI(LOG_TAG,  "switching screen OFF, new mCurrentState: %d", new_screen_state);
          if (power_save_enabled)
          {
            ESP_LOGI(LOG_TAG,  "setting CPU freq to: %d", power_save_freq_mhz);
            setCpuFrequencyMhz(power_save_freq_mhz);
          }
        }
        else{
          ESP_LOGI(LOG_TAG,  "switching screen ON, new mCurrentState: %d", new_screen_state);
          if (power_save_enabled)
          {
            ESP_LOGI(LOG_TAG,  "setting CPU freq to: %d", performance_freq_mhz);
            setCpuFrequencyMhz(performance_freq_mhz);
          }
        }

        ScreenOnOffSwitch(mpDriver, new_screen_state, mpCtx->getModel().mHardwareConfig.mIsLEDPinInverted);
        mCurrentState = new_screen_state;
        
        return mCurrentState;
      }

      std::chrono::system_clock::time_point mLastTouch;
      const bool screen_on = false;
      const bool screen_off = true;
      bool mCurrentState = screen_on;  
      int last_log_print_sec = 0;
      int power_save_freq_mhz = 80;
      int performance_freq_mhz = 240;
      bool power_save_enabled = true;
      ScreenDriver* mpDriver;
      std::shared_ptr<ctx::AppContext> mpCtx;

  };
} // namespace gfx
