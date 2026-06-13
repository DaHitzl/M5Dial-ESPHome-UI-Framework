#pragma once
#include "page_interface.h"
#include "ui_helpers.h"
#include <vector>
#include <string>

class HAPage : public PageInterface {
private:
  int level = 0;
  int selected = 0;
  int offset = 0;
  std::vector<std::string> rooms;
  std::vector<std::string> devices;
  std::string entity = "";

public:
  std::function<void(std::string)> on_room_selected;
  std::function<void(std::string, int)> on_action_requested;

  void update_rooms(std::vector<std::string> r) { rooms = r; }
  void update_devices(std::vector<std::string> d) { devices = d; }
  
  void reset_to_devices() {
    entity = ""; level = 2; selected = 0; offset = 0;
  }
  
  int get_level() { return level; }

  // --- DIE DREI FEHLENDEN GETTER FÜR DIE YAML ---
  std::string get_selected_room() {
    if (rooms.empty() || selected >= rooms.size()) return "";
    std::string raum = rooms[selected];
    std::transform(raum.begin(), raum.end(), raum.begin(), ::tolower);
    return raum;
  }
  
  std::string get_selected_entity() { return entity; }
  
  int get_action_type() { return selected; }
  // ----------------------------------------------

  void draw(esphome::display::Display &it) override {
    it.image(0, 0, &id(img_ha_nof));
    UIHelpers::draw_header(it, "Home Assistant", Color(0, 180, 255));

    const char* untertext = "";
    std::vector<std::string> eintraege;

    switch(level) {
      case 0: untertext = "[ Etage ]"; eintraege = {"Erdgeschoss", "Obergeschoss", "Aussenbereich"}; break;
      case 1: untertext = "[ Raum ]"; eintraege = rooms.empty() ? std::vector<std::string>{"Lade Räume..."} : rooms; break;
      case 2:
        untertext = "[ Gerät ]";
        if (!devices.empty()) {
          for(auto& raw_dev : devices) {
            size_t hash1 = raw_dev.find('#');
            size_t hash2 = raw_dev.find('#', hash1 + 1);
            if (hash1 != std::string::npos && hash2 != std::string::npos)
              eintraege.push_back(raw_dev.substr(hash1 + 1, hash2 - hash1 - 1) + " [" + raw_dev.substr(hash2 + 1) + "]");
            else if (hash1 != std::string::npos) eintraege.push_back(raw_dev.substr(hash1 + 1));
            else eintraege.push_back(raw_dev); 
          }
        } else eintraege = {"Lade Geräte..."};
        break;
      case 3: untertext = "[ Funktion ]"; eintraege = {"Umschalten", "Einschalten", "Ausschalten"}; break;
    }

    it.print(120, 68, &id(meine_schrift), Color(150, 150, 150), TextAlign::CENTER, untertext);
    UIHelpers::draw_menu(it, eintraege, selected, offset, 85, 28);
  }

  void on_encoder(int step) override {
    int total = 3;
    if (level == 1) total = rooms.empty() ? 1 : rooms.size();
    else if (level == 2) total = devices.empty() ? 1 : devices.size();
    UIHelpers::move_selection(step, selected, offset, total, 3);
  }

  void on_button() override {
    switch(level) {
      case 0: level = 1; selected = 0; offset = 0; break;
      case 1: 
        if (!rooms.empty() && on_room_selected) {
          std::string raum = rooms[selected];
          std::transform(raum.begin(), raum.end(), raum.begin(), ::tolower);
          on_room_selected(raum);
        }
        break;
      case 2:
        if (!devices.empty() && devices[0] != "Keine Geräte") {
          std::string raw_dev = devices[selected];
          size_t hash_pos = raw_dev.find('#');
          if (hash_pos != std::string::npos) {
            entity = raw_dev.substr(0, hash_pos);
            level = 3; selected = 0; offset = 0;
          }
        }
        break;
      case 3: 
        if (!entity.empty() && on_action_requested) {
          on_action_requested(entity, selected);
        }
        break;
    }
  }

  void go_back() {
    if (level > 0) { level--; selected = 0; offset = 0; }
  }
};