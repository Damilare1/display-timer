#ifndef LGFX_h
#define LGFX_h

#define LGFX_USE_V1

class LGFX : public lgfx::LGFX_Device {
public:
  lgfx::Panel_ILI9341 _panel_instance;
  lgfx::Bus_SPI _bus_instance;  // SPI bus instance // SPIバスのインスタンス
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;

public:
  LGFX(void);
};

#endif