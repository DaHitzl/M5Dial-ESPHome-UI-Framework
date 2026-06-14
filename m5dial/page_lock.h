#pragma once
#include "page_interface.h"
#include "ui_helpers.h"
#include <functional>

class LockPage : public PageInterface {
public:
  int pin_digits[4] = {0, 0, 0, 0};
  int pin_cursor = 0;
  int target_pin_val = 0; 
  
  std::function<void()> on_unlocked;
  std::function<void()> on_cancel;

  void setup(int pin) {
     target_pin_val = pin;
     pin_cursor = 0;
     for(int i=0; i<4; i++) pin_digits[i] = 0;
  }

  void draw(esphome::display::Display &it) override {
     it.fill(Color(0,0,0)); 
     UIHelpers::draw_header(it, "Gesperrt", Color(255, 0, 0));
     it.print(120, 65, &id(meine_schrift), Color(180, 180, 180), TextAlign::CENTER, "Display Code:");
     
     int start_x = 75;
     for(int i=0; i<4; i++) {
       Color c = (i == pin_cursor) ? Color(0, 255, 255) : Color(255, 255, 255);
       if (i == pin_cursor) {
           it.printf(start_x + (i*30), 110, &id(meine_schrift), c, TextAlign::CENTER, "[%d]", pin_digits[i]);
       } else if (i < pin_cursor) {
           it.printf(start_x + (i*30), 110, &id(meine_schrift), Color(180,180,180), TextAlign::CENTER, "%d", pin_digits[i]);
       } else {
           it.print(start_x + (i*30), 110, &id(meine_schrift), Color(100,100,100), TextAlign::CENTER, "_");
       }
     }
     
     it.print(30, 200, &id(meine_schrift), Color(150,150,150), TextAlign::LEFT, "< Abbrechen");
  }

  void on_encoder(int step) override {
     int v = pin_digits[pin_cursor] + step;
     if(v > 9) v = 0;
     if(v < 0) v = 9;
     pin_digits[pin_cursor] = v;
  }

  void on_button() override {
     pin_cursor++;
     if (pin_cursor < 4) {
         pin_digits[pin_cursor] = pin_digits[pin_cursor-1]; 
     } else {
         int entered = pin_digits[0]*1000 + pin_digits[1]*100 + pin_digits[2]*10 + pin_digits[3];
         if (entered == target_pin_val) {
             if (on_unlocked) on_unlocked();
         } else {
             pin_cursor = 0; 
             pin_digits[0] = 0;
         }
     }
  }
  
  void on_touch(int x, int y) override {
      if (y > 170 && x < 100) { 
          if (on_cancel) on_cancel();
      }
  }
};