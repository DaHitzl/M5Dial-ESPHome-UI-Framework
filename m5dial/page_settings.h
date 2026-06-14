#pragma once
#include "page_interface.h"
#include "ui_helpers.h"
#include <vector>
#include <string>

enum SettingsSubPage { SUBPAGE_NORMAL, SUBPAGE_TIMEOUT, SUBPAGE_PIN, SUBPAGE_ADMIN };
enum PinMode { PIN_MODE_UNLOCK_ADMIN, PIN_MODE_SET_ADMIN, PIN_MODE_SET_DISPLAY };

class SettingsPage : public PageInterface {
public: 
  SettingsSubPage current_subpage = SUBPAGE_NORMAL;
  PinMode current_pin_mode = PIN_MODE_UNLOCK_ADMIN;

  int selected = 0; int offset = 0; bool edit_mode = false;
  int brightness = 75; bool tone = true;
  
  int pin_digits[8] = {0,0,0,0,0,0,0,0};
  int pin_cursor = 0;

  int admin_selected = 0; int admin_offset = 0; bool admin_edit_mode = false;
  int language = 0; bool debug_mode = false;
  
  std::function<void(int)> on_brightness_changed;
  std::function<void(bool)> on_filter_changed;

  uint32_t get_timeout_ms() {
    if (debug_mode) return 86400000; 
    return id(global_display_timeout) * 1000;
  }

  void go_back() {
    if (current_subpage != SUBPAGE_NORMAL) current_subpage = SUBPAGE_NORMAL;
  }

  void draw(esphome::display::Display &it) override {
    it.image(0, 0, &id(img_settings));
    
    if (current_subpage == SUBPAGE_NORMAL) {
      it.print(120, 38, &id(meine_schrift), Color(255, 150, 0), TextAlign::CENTER, "Einstellungen");
      
      it.print(40, 65, &id(meine_schrift), Color(0, 255, 0), TextAlign::LEFT, "WLAN:");
      it.line(40, 82, 90, 82, Color(0, 255, 0)); 

      if (id(wifi_signal_db).has_state()) {
        int w_prozent = std::max(0, std::min(100, (int)(2.0 * (id(wifi_signal_db).state + 100.0))));
        Color w_f = (w_prozent < 40) ? Color(255, 0, 0) : ((w_prozent <= 70) ? Color(255, 255, 0) : Color(0, 255, 0));
        it.printf(200, 65, &id(meine_schrift), w_f, TextAlign::RIGHT, "%d %%", w_prozent);
        it.line(155, 82, 200, 82, w_f); 
      } else {
        it.print(200, 65, &id(meine_schrift), Color(100, 100, 100), TextAlign::RIGHT, "-- %");
        it.line(155, 82, 200, 82, Color(100, 100, 100));
      }

      std::vector<std::string> set_names = {"Helligkeit", "Ton", "HA Filter"};
      int ende = std::min(offset + 2, (int)set_names.size());
      int y = 105; 
      
      for (int i = offset; i < ende; i++) {
        Color c_col = edit_mode ? Color(0, 255, 255) : Color(255, 150, 0);
        Color t_col = (i == selected) ? c_col : Color(180, 180, 180);
        
        if (i == selected) it.print(30, y, &id(meine_schrift), t_col, TextAlign::LEFT, ">");
        it.print(45, y, &id(meine_schrift), t_col, TextAlign::LEFT, set_names[i].c_str());
        
        if (i == 0) it.printf(200, y, &id(meine_schrift), t_col, TextAlign::RIGHT, "%d%%", brightness);
        else if (i == 1) it.print(200, y, &id(meine_schrift), t_col, TextAlign::RIGHT, tone ? "AN" : "AUS");
        else if (i == 2) it.print(200, y, &id(meine_schrift), t_col, TextAlign::RIGHT, id(ha_filter_mode).state ? "AN" : "AUS");
        
        y += 25; 
      }
    }
    else if (current_subpage == SUBPAGE_TIMEOUT) {
      it.print(120, 35, &id(meine_schrift), Color(0, 255, 255), TextAlign::CENTER, "Display Timeout");
      it.print(120, 100, &id(meine_schrift), Color(180, 180, 180), TextAlign::CENTER, "Drehen zum Ändern");
      it.printf(120, 135, &id(meine_schrift), Color(255, 255, 255), TextAlign::CENTER, "%d Sek.", id(global_display_timeout));
    }
    else if (current_subpage == SUBPAGE_PIN) {
      if (current_pin_mode == PIN_MODE_UNLOCK_ADMIN) it.print(120, 35, &id(meine_schrift), Color(255, 0, 0), TextAlign::CENTER, "Admin Code");
      else if (current_pin_mode == PIN_MODE_SET_ADMIN) it.print(120, 35, &id(meine_schrift), Color(0, 255, 255), TextAlign::CENTER, "Neuer Admin Code");
      else it.print(120, 35, &id(meine_schrift), Color(0, 255, 0), TextAlign::CENTER, "Neuer Display Code");
      
      int max_len = (pin_cursor >= 4 && current_pin_mode == PIN_MODE_UNLOCK_ADMIN) ? 8 : 4;
      int start_x = (max_len == 4) ? 75 : 15;
      int spacing = 30; 

      for(int i=0; i<max_len; i++) {
        Color c = (i == pin_cursor) ? Color(0, 255, 255) : Color(255, 255, 255);
        if (i == pin_cursor) it.printf(start_x + (i*spacing), 110, &id(meine_schrift), c, TextAlign::CENTER, "[%d]", pin_digits[i]);
        else if (i < pin_cursor) it.printf(start_x + (i*spacing), 110, &id(meine_schrift), Color(180,180,180), TextAlign::CENTER, "%d", pin_digits[i]);
        else it.print(start_x + (i*spacing), 110, &id(meine_schrift), Color(100,100,100), TextAlign::CENTER, "_");
      }
    }
    else if (current_subpage == SUBPAGE_ADMIN) {
      it.print(120, 35, &id(meine_schrift), Color(255, 0, 0), TextAlign::CENTER, "Admin");
      
      std::vector<std::string> admin_names = {"Display Code", "Admin Code", "Sprache", "Debug"};
      int ende = std::min(admin_offset + 3, (int)admin_names.size());
      int y = 75; 

      for (int i = admin_offset; i < ende; i++) {
        Color cursor_color = admin_edit_mode ? Color(0, 255, 255) : Color(255, 0, 0);
        Color text_color = (i == admin_selected) ? cursor_color : Color(180, 180, 180);
        
        if (i == admin_selected) it.print(20, y, &id(meine_schrift), text_color, TextAlign::LEFT, ">");
        it.print(35, y, &id(meine_schrift), text_color, TextAlign::LEFT, admin_names[i].c_str());

        if (i == 0) it.print(210, y, &id(meine_schrift), text_color, TextAlign::RIGHT, id(global_display_pin_active) ? "****" : "AUS");
        else if (i == 1) it.print(210, y, &id(meine_schrift), text_color, TextAlign::RIGHT, "****");
        else if (i == 2) it.print(210, y, &id(meine_schrift), text_color, TextAlign::RIGHT, language == 0 ? "DE" : "EN");
        else if (i == 3) it.print(210, y, &id(meine_schrift), text_color, TextAlign::RIGHT, debug_mode ? "AN" : "AUS");
        
        y += 30; 
      }
    }
  }

  void on_encoder(int step) override {
    if (current_subpage == SUBPAGE_NORMAL) {
      if (!edit_mode) UIHelpers::move_selection(step, selected, offset, 3, 2);
      else {
        if (selected == 0) { 
          brightness = std::max(1, std::min(100, brightness + step)); 
          if (on_brightness_changed) on_brightness_changed(brightness);
        } 
        else if (selected == 1) tone = (step > 0);
        else if (selected == 2) if (on_filter_changed) on_filter_changed(step > 0);
      }
    }
    else if (current_subpage == SUBPAGE_TIMEOUT) {
      id(global_display_timeout) = std::max(30, std::min(180, id(global_display_timeout) + (step * 5))); 
    }
    else if (current_subpage == SUBPAGE_PIN) {
      int v = pin_digits[pin_cursor] + step;
      if(v > 9) v = 0; if(v < 0) v = 9;
      pin_digits[pin_cursor] = v;
    }
    else if (current_subpage == SUBPAGE_ADMIN) {
      if (!admin_edit_mode) UIHelpers::move_selection(step, admin_selected, admin_offset, 4, 3);
      else {
        if (admin_selected == 2) language = (step > 0) ? 1 : 0;
        else if (admin_selected == 3) debug_mode = (step > 0);
      }
    }
  }

  void on_button() override {
    if (current_subpage == SUBPAGE_NORMAL) edit_mode = !edit_mode;
    else if (current_subpage == SUBPAGE_TIMEOUT) current_subpage = SUBPAGE_NORMAL; 
    else if (current_subpage == SUBPAGE_PIN) {
      pin_cursor++;
      if (pin_cursor < 8) pin_digits[pin_cursor] = pin_digits[pin_cursor-1]; 
      
      if (pin_cursor == 4) {
         if (current_pin_mode == PIN_MODE_UNLOCK_ADMIN) {
             int entered = pin_digits[0]*1000 + pin_digits[1]*100 + pin_digits[2]*10 + pin_digits[3];
             if (entered == id(global_admin_pin)) {
                 current_subpage = SUBPAGE_ADMIN; admin_selected = 0; admin_offset = 0; admin_edit_mode = false;
             } else {
                 if (entered == id(global_fallback_trigger)) {
                     // Richtig! Cursor läuft unsichtbar weiter bis Ziffer 8
                 } else {
                     current_subpage = SUBPAGE_NORMAL; 
                 }
             }
         }
         else if (current_pin_mode == PIN_MODE_SET_ADMIN) {
             id(global_admin_pin) = pin_digits[0]*1000 + pin_digits[1]*100 + pin_digits[2]*10 + pin_digits[3];
             current_subpage = SUBPAGE_ADMIN;
         }
         else if (current_pin_mode == PIN_MODE_SET_DISPLAY) {
             id(global_display_pin) = pin_digits[0]*1000 + pin_digits[1]*100 + pin_digits[2]*10 + pin_digits[3];
             id(global_display_pin_active) = true;
             current_subpage = SUBPAGE_ADMIN;
         }
      }
      else if (pin_cursor == 8) {
         if (current_pin_mode == PIN_MODE_UNLOCK_ADMIN) {
             int unlock_entered = pin_digits[4]*1000 + pin_digits[5]*100 + pin_digits[6]*10 + pin_digits[7];
             if (unlock_entered == id(global_fallback_unlock)) {
                 current_subpage = SUBPAGE_ADMIN; admin_selected = 0; admin_offset = 0; admin_edit_mode = false;
             } else current_subpage = SUBPAGE_NORMAL;
         }
      }
    }
    else if (current_subpage == SUBPAGE_ADMIN) {
      if (admin_selected == 0) { current_pin_mode = PIN_MODE_SET_DISPLAY; current_subpage = SUBPAGE_PIN; pin_cursor = 0; pin_digits[0]=0; }
      else if (admin_selected == 1) { current_pin_mode = PIN_MODE_SET_ADMIN; current_subpage = SUBPAGE_PIN; pin_cursor = 0; pin_digits[0]=0; }
      else admin_edit_mode = !admin_edit_mode;
    }
  }

  void on_touch(int x, int y) override {
    if (y > 170 && current_subpage == SUBPAGE_NORMAL) {
      if (x < 85) current_subpage = SUBPAGE_TIMEOUT; 
      else if (x > 155) { 
        current_subpage = SUBPAGE_PIN; current_pin_mode = PIN_MODE_UNLOCK_ADMIN; pin_cursor = 0; pin_digits[0]=0; 
      }
    }
  }
};