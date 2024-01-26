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
          return switchScreen(false);
        }
        else
        {
          return switchScreen(true);
        }
      }

      template<typename T>
      bool tapped(const T& tapEvt)
      {
        if (!tapEvt)
        {
          return mCurrentState;
        }
        auto tapEvent = *tapEvt;
        if (tapEvent.state == decltype(tapEvent.state)::Tap)
        {
          mLastTouch = std::chrono::system_clock::now();
        }
        return mCurrentState;
      }

      void activate()
      {
        mLastTouch = std::chrono::system_clock::now();
      }

    private:

      bool switchScreen(bool on)
      {
        if (on == mCurrentState)
        {
          return mCurrentState;
        }
        
        if (on)
        {
          ESP_LOGI(LOG_TAG,  "switching screen OFF, new state: %d", on);
        }
        else{
          ESP_LOGI(LOG_TAG,  "switching screen ON, new state: %d", on);
        }

        ScreenOnOffSwitch(mpDriver, on, mpCtx->getModel().mHardwareConfig.mIsLEDPinInverted);
        mCurrentState = on;
        
        return mCurrentState;
      }

      std::chrono::system_clock::time_point mLastTouch;
      bool mCurrentState = false;
      int last_log_print_sec = 0;
      ScreenDriver* mpDriver;
      std::shared_ptr<ctx::AppContext> mpCtx;

  };
} // namespace gfx
