#pragma once
#include "page_interface.h"
#include "ui_helpers.h"
#include <vector>
#include <string>

class SettingsPage : public PageInterface {
public: // Public für schnellen Lesezugriff
  int selected = 0;
  int offset = 0;
  bool edit_mode = false;
  int brightness = 75;
  bool tone = true;
  
  // Callbacks
  std::function<void(int)> on_brightness_changed;
  std::function<void(bool)> on_filter_changed;

  void draw(esphome::display::Display &it) override {
    it.image(0, 0, &id(img_settings));
    UIHelpers::draw_header(it, "Einstellungen", Color(255, 150, 0));
    it.print(40, 65, &id(meine_schrift), Color(0, 255, 0), TextAlign::LEFT, "WLAN:");
    
    if (id(wifi_signal_db).has_state()) {
      int w_prozent = std::max(0, std::min(100, (int)(2.0 * (id(wifi_signal_db).state + 100.0))));
      Color w_farbe = (w_prozent < 40) ? Color(255, 0, 0) : ((w_prozent <= 70) ? Color(255, 255, 0) : Color(0, 255, 0));
      it.printf(200, 65, &id(meine_schrift), w_farbe, TextAlign::RIGHT, "%d %%", w_prozent);
    } else {
      it.print(200, 65, &id(meine_schrift), Color(100, 100, 100), TextAlign::RIGHT, "-- %");
    }

    std::vector<std::string> set_names = {"Helligkeit", "Ton", "Filter"};
    int ende = std::min(offset + 2, (int)set_names.size());
    int y = 105; 

    for (int i = offset; i < ende; i++) {
      Color c = (i == selected) ? (edit_mode ? Color(0, 255, 255) : Color(255, 150, 0)) : Color(180, 180, 180);
      if (i == selected) it.print(30, y, &id(meine_schrift), c, TextAlign::LEFT, ">");
      it.print(45, y, &id(meine_schrift), c, TextAlign::LEFT, set_names[i].c_str());

      if (i == 0) it.printf(200, y, &id(meine_schrift), c, TextAlign::RIGHT, "%d%%", brightness);
      else if (i == 1) it.print(200, y, &id(meine_schrift), c, TextAlign::RIGHT, tone ? "AN" : "AUS");
      else if (i == 2) it.print(200, y, &id(meine_schrift), c, TextAlign::RIGHT, id(ha_filter_mode).state ? "AN" : "AUS");
      y += 35; 
    }
  }

  void on_encoder(int step) override {
    if (!edit_mode) {
      UIHelpers::move_selection(step, selected, offset, 3, 2);
    } else {
      if (selected == 0) { 
        brightness = std::max(5, std::min(100, brightness + step));
        if (on_brightness_changed) on_brightness_changed(brightness);
      } 
      else if (selected == 1) tone = (step > 0);
      else if (selected == 2) { 
        if (on_filter_changed) on_filter_changed(step > 0);
      }
    }
  }

  void on_button() override {
    if (selected <= 2) {
      edit_mode = !edit_mode;
    }
  }
};