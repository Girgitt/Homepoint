#pragma once

#include <stdint.h>

namespace gfx
{
  struct Color
  {
    int r;
    int g;
    int b;

    uint16_t getColorInt()
    {
      return static_cast<uint16_t>((r*31/255 >> 0) << 11) | ((g*63/255 >> 0) << 5) | ((b*31/255) >> 0); //OK
    }

    static Color BlackColor()
    {
      return {0, 0, 0};
    }

    static Color WhiteColor()
    {
      return {255, 255, 255};
    }

    static Color LightGrayColor()
    {
      return {114, 126, 145};
    }

    static Color StormCloudColor()
    {
    return {79, 102, 106};
    }

    static Color DarkGreenColor()
    {
      return {0, 102, 102};
    }

    static Color ActiveBgColor()
    {
      return {224, 211, 96};
    }

    static Color ActiveTextColor()
    {
      return {0, 0, 0};
    }

    static Color InactiveBgColor()
    {
      return {0, 0, 0};
    }

    static Color InactiveTextColor()
    {
      return {255, 255, 255};
    }

    static Color SelectedColor()
    {
      return {185, 244, 66};
    }
  };

} // namespace gfx
