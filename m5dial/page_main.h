#pragma once
#include "page_interface.h"

class MainPage : public PageInterface {
public:
  // Callback-Definition
  std::function<void(int)> on_page_change_requested;

  void draw(esphome::display::Display &it) override {
    it.image(0, 0, &id(img_main));
    auto now = id(ha_time).now();
    if (now.is_valid()) {
      it.printf(120, 35, &id(meine_schrift), Color(255, 255, 0), TextAlign::CENTER, "%02d:%02d", now.hour, now.minute);
      it.printf(120, 60, &id(meine_schrift), Color(180, 180, 180), TextAlign::CENTER, "%02d.%02d.%04d", now.day_of_month, now.month, now.year);
    }
    if (id(temp_sensor).has_state()) it.printf(120, 95, &id(meine_schrift), Color(255, 255, 255), TextAlign::CENTER, "%.1f °C", id(temp_sensor).state);
    else it.print(120, 95, &id(meine_schrift), Color(100, 100, 100), TextAlign::CENTER, "--.- °C");
    
    if (id(humidity_sensor).has_state()) it.printf(120, 125, &id(meine_schrift), Color(255, 255, 255), TextAlign::CENTER, "%.1f %%", id(humidity_sensor).state);
    else it.print(120, 125, &id(meine_schrift), Color(100, 100, 100), TextAlign::CENTER, "--.- %%");
  }

  void on_touch(int x, int y) override {
    if (y > 130 && on_page_change_requested) {
      if (x < 85) on_page_change_requested(1); // HA
      else if (x > 155) on_page_change_requested(2); // Settings
      else on_page_change_requested(3); // Music
    }
  }
};