#pragma once
#include "esphome.h"
#include "sound_manager.h"
#include "page_main.h"
#include "page_ha.h"
#include "page_settings.h"
#include "page_music.h"

class M5DialUI {
private:
  MainPage main_page;
  HAPage ha_page;
  SettingsPage settings_page;
  MusicPage music_page;

  PageInterface* current_page;
  int current_page_index = 0;

  bool display_awake = true;
  uint32_t last_activity = 0;

  void reset_timer() { last_activity = millis(); }

  void change_page(int page_idx) {
    current_page_index = page_idx;
    switch(page_idx) {
      case 0: current_page = &main_page; break;
      case 1: current_page = &ha_page; break;
      case 2: current_page = &settings_page; break;
      case 3: current_page = &music_page; break;
    }
    id(update_display_delayed).execute();
  }

public:
  M5DialUI() {
    current_page = &main_page;
    
    // VERDRAHTUNG MAIN
    main_page.on_page_change_requested = [this](int new_page) {
      SoundManager::play(SOUND_TOUCH, settings_page.tone);
      this->change_page(new_page);
    };

    // VERDRAHTUNG HA
    ha_page.on_room_selected = [this](std::string raum) {
      id(script_ha_select_room).execute(); 
    };

    ha_page.on_action_requested = [this](std::string entity, int action_type) {
      id(script_ha_execute_action).execute();
    };

    // VERDRAHTUNG SETTINGS
    settings_page.on_brightness_changed = [this](int bright) {
      auto call = id(dial_backlight).turn_on();
      call.set_brightness(bright / 100.0);
      call.perform();
    };

    settings_page.on_filter_changed = [this](bool enable) {
      if (enable) {
        id(filter_on_script).execute();
        id(ha_filter_mode).publish_state(true);
      } else {
        id(filter_off_script).execute();
        id(ha_filter_mode).publish_state(false);
      }
    };
  } // <-- Hier ist die magische Klammer, die vorher verrutscht war!

  HAPage* get_ha_page() { return &ha_page; }

  void check_sleep() {
    if (display_awake && (millis() - last_activity > 60000)) {
      display_awake = false;
      change_page(0);
      id(dial_backlight).turn_off().perform();
    }
  }

  void on_encoder(int step) {
    if (step == 0) return;
    reset_timer();

    if (!display_awake) {
      display_awake = true;
      auto call = id(dial_backlight).turn_on();
      call.set_brightness(settings_page.brightness / 100.0);
      call.perform();
      return;
    }

    if (current_page_index == 1 || current_page_index == 2) {
      SoundManager::play(step > 0 ? SOUND_RIGHT : SOUND_LEFT, settings_page.tone);
      current_page->on_encoder(step);
      id(update_display_delayed).execute();
    }
  }

  void on_button() {
    reset_timer();
    if (!display_awake) {
      display_awake = true;
      auto call = id(dial_backlight).turn_on();
      call.set_brightness(settings_page.brightness / 100.0);
      call.perform();
      SoundManager::play(SOUND_TOUCH, settings_page.tone);
      return;
    }

    SoundManager::play(SOUND_BUTTON, settings_page.tone);
    current_page->on_button();
    id(update_display_delayed).execute();
  }

  void on_touch(int x, int y) {
    reset_timer();
    if (!display_awake) {
      display_awake = true;
      auto call = id(dial_backlight).turn_on();
      call.set_brightness(settings_page.brightness / 100.0);
      call.perform();
      return; 
    }

    if (current_page_index == 0) {
      current_page->on_touch(x, y); 
    } 
    else if (current_page_index > 0) {
      if (y > 165 && x >= 90 && x <= 150) { 
        SoundManager::play(SOUND_BACK, settings_page.tone);
        if (current_page_index == 1 && ha_page.get_level() > 0) {
           ha_page.go_back();
        } else {
           change_page(0); 
        }
        id(update_display_delayed).execute();
      }
      else {
        current_page->on_touch(x, y);
      }
    }
  }

  void render(esphome::display::Display &it) {
    current_page->draw(it);
  }
};

M5DialUI *dial_ui = new M5DialUI();