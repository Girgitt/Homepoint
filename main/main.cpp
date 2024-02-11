
#define ESP_PLATFORM 1
#include <Arduino.h>
#include <SPI.h>
#include <config/Config.h>
#include <fs/ConfigReader.hpp>
#include "AppScreen.hpp"
#include "SharedGlobalState.h"

#include <memory>
#include "esp_log.h"

extern "C"
{
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "esp_task_wdt.h"
  #include "nvs_flash.h"
  
}

#define MAINLOOPCORE 0
TaskHandle_t runLoopHandle = NULL;
bool loopTaskWDTEnabled = true; // Enable if watchdog running

std::shared_ptr<ctx::AppContext> mpAppContext(new ctx::AppContext());
gfx::AppScreen<ScreenDriver, NavigationDriver> mScreen(mpAppContext);
//std::shared_ptr<sgs::SharedGlobalState> global_state = sgs::SharedGlobalState::getInstance();

extern "C" 
{
  void runLoop(void *pvParameters);
  void setupApp();
  
  void* sharedGlobalStateGetInstance();
  void  sharedGlobalStateTickUptimeMs(void* instance, int ms);
  void* sgs_instance = sharedGlobalStateGetInstance();

  void app_main()
  {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    initArduino();
    Serial.begin(115200);

    try
    {
      mpAppContext->setup();
    }
    catch(const std::exception& e)
    {
      mScreen.showWarning(e.what());
      mScreen.registerWifiCallback();
      return;
    }
    
    InitializePlatform(mpAppContext->getModel().mHardwareConfig);
    
    setupApp();
  }

  void setupApp()
  {
    
    mScreen.setupScreen();
    xTaskCreateUniversal(runLoop, "loopTask", 4096, NULL, 1, &runLoopHandle, MAINLOOPCORE);
    mScreen.showWarning("Initializing HomePoint");

    if (!mpAppContext->getModel().hasWifiCredentials())
    {
      mScreen.showWarning("AP: HomePoint-Config, IP: 192.168.99.1");
      return;
    }
    mScreen.showWarning("Loading Screen");
    mScreen.setupData();
  }

  void runLoop(void *pvParameters)
  {
    //esp_log_level_set("*", ESP_LOG_INFO);
    int desired_loop_cycle_length_ms = 30;
    ESP_LOGI("main", "starting main loop");
    long loop_start_ts;
    long loop_end_ts = loop_start_ts = millis();
    int work_took_ms = 0;
    int loop_time_with_delays_took_ms = 0;
    int loops_cnt = 0;
    
    for(;;)
    {
        loop_start_ts = millis();
        
        if (loopTaskWDTEnabled)
        {
          esp_task_wdt_reset();
        }
        
        mScreen.draw();
        
        sharedGlobalStateTickUptimeMs(sgs_instance, millis() - loop_end_ts);
        
        loop_end_ts = millis();
        
        //global_state->tickUptimeMs(desired_loop_cycle_length_ms);
        work_took_ms = loop_end_ts - loop_start_ts; 
        
        if (work_took_ms < desired_loop_cycle_length_ms)
        {
          delay(desired_loop_cycle_length_ms-work_took_ms);
        }
        else
        {
          ESP_LOGI("main", "work_took_ms > desired_loop_cycle_length_ms");
          ESP_LOGI("main", "work_took_ms: %d", work_took_ms);
          delay(15);  //we still need 15ms to be sure next touch read is available
        }

        loop_time_with_delays_took_ms = millis() - loop_start_ts;
        
        if(loops_cnt % 100 == 0)
        {
          ESP_LOGI("main", "work_took_ms: %d", work_took_ms);
          ESP_LOGI("main", "loop_time_with_delays_took_ms: %d", loop_time_with_delays_took_ms);
        }
        loops_cnt += 1;
    }
  }
}
