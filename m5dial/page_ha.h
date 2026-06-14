#pragma once
#include "page_interface.h"
#include "ui_helpers.h"
#include <vector>
#include <string>

enum EntityDomain { DOMAIN_DEFAULT, DOMAIN_LIGHT, DOMAIN_CLIMATE, DOMAIN_FAN };

class HAPage : public PageInterface {
private:
  int level = 0;
  int selected = 0;
  int offset = 0;
  std::vector<std::string> rooms;
  std::vector<std::string> devices;
  std::string entity = "";
  
  // Neue Logik für dynamische Menüs
  EntityDomain current_domain = DOMAIN_DEFAULT;
  bool edit_mode = false;
  int param_value = 0; // Speichert den aktuellen Wert beim Drehen

public:
  std::function<void(std::string)> on_room_selected;
  // NEU: Callback gibt jetzt Entität, Befehls-String und einen Wert mit
  std::function<void(std::string, std::string, int)> on_advanced_action;

  void update_rooms(std::vector<std::string> r) { rooms = r; }
  void update_devices(std::vector<std::string> d) { devices = d; }
  
  void reset_to_devices() {
    entity = ""; level = 2; selected = 0; offset = 0; edit_mode = false;
  }
  
  int get_level() { return level; }
  std::string get_selected_room() {
    if (rooms.empty() || selected >= rooms.size()) return "";
    std::string raum = rooms[selected];
    std::transform(raum.begin(), raum.end(), raum.begin(), ::tolower);
    return raum;
  }

  void draw(esphome::display::Display &it) override {
    it.image(0, 0, &id(img_ha_nof));
    UIHelpers::draw_header(it, "Home Assistant", Color(0, 180, 255));

    const char* untertext = "";
    std::vector<std::string> eintraege;

    if (level == 0) { untertext = "[ Etage ]"; eintraege = {"Erdgeschoss", "Obergeschoss", "Aussenbereich"}; }
    else if (level == 1) { untertext = "[ Raum ]"; eintraege = rooms.empty() ? std::vector<std::string>{"Lade Räume..."} : rooms; }
    else if (level == 2) {
      untertext = "[ Gerät ]";
      if (!devices.empty()) {
        for(auto& raw_dev : devices) {
          size_t hash1 = raw_dev.find('#'); size_t hash2 = raw_dev.find('#', hash1 + 1);
          if (hash1 != std::string::npos && hash2 != std::string::npos) eintraege.push_back(raw_dev.substr(hash1 + 1, hash2 - hash1 - 1) + " [" + raw_dev.substr(hash2 + 1) + "]");
          else if (hash1 != std::string::npos) eintraege.push_back(raw_dev.substr(hash1 + 1));
          else eintraege.push_back(raw_dev); 
        }
      } else eintraege = {"Lade Geräte..."};
    }
    else if (level == 3) {
      // DYNAMISCHES MENÜ BASIEREND AUF GERÄTETYP
      if (current_domain == DOMAIN_LIGHT) { untertext = "[ Licht ]"; eintraege = {"An / Aus", "Helligkeit", "Farbe"}; }
      else if (current_domain == DOMAIN_CLIMATE) { untertext = "[ Thermostat ]"; eintraege = {"Temperatur", "Heizen", "Ausschalten"}; }
      else if (current_domain == DOMAIN_FAN) { untertext = "[ Lüfter ]"; eintraege = {"An / Aus", "Stufe"}; }
      else { untertext = "[ Schalter ]"; eintraege = {"Umschalten", "Einschalten", "Ausschalten"}; }
    }

    it.print(120, 68, &id(meine_schrift), Color(150, 150, 150), TextAlign::CENTER, untertext);

    // ZEICHNEN DER LISTE ODER DES EDIT-MODUS
    int ende = std::min(offset + 3, (int)eintraege.size());
    int y = 95; 

    for (int i = offset; i < ende; i++) {
      Color c_col = edit_mode ? Color(0, 255, 255) : Color(255, 255, 0);
      Color t_col = (i == selected) ? c_col : Color(180, 180, 180);
      
      if (i == selected) it.print(30, y, &id(meine_schrift), t_col, TextAlign::LEFT, ">");
      it.print(45, y, &id(meine_schrift), t_col, TextAlign::LEFT, eintraege[i].c_str());

      // LIVE WERT ANZEIGEN IM EDIT MODUS (rechtsbündig)
      if (level == 3 && edit_mode && i == selected) {
         if (current_domain == DOMAIN_LIGHT && selected == 1) it.printf(210, y, &id(meine_schrift), t_col, TextAlign::RIGHT, "%d%%", param_value);
         else if (current_domain == DOMAIN_LIGHT && selected == 2) it.printf(210, y, &id(meine_schrift), t_col, TextAlign::RIGHT, "%d°", param_value); // Farbton 0-360
         else if (current_domain == DOMAIN_CLIMATE && selected == 0) it.printf(210, y, &id(meine_schrift), t_col, TextAlign::RIGHT, "%d°C", param_value);
         else if (current_domain == DOMAIN_FAN && selected == 1) it.printf(210, y, &id(meine_schrift), t_col, TextAlign::RIGHT, "%d%%", param_value);
      }
      y += 26;
    }
  }

  void on_encoder(int step) override {
    if (level < 3 || !edit_mode) {
      int total = 3;
      if (level == 1) total = rooms.empty() ? 1 : rooms.size();
      else if (level == 2) total = devices.empty() ? 1 : devices.size();
      else if (level == 3) {
          if (current_domain == DOMAIN_DEFAULT || current_domain == DOMAIN_LIGHT || current_domain == DOMAIN_CLIMATE) total = 3;
          else if (current_domain == DOMAIN_FAN) total = 2;
      }
      UIHelpers::move_selection(step, selected, offset, total, 3);
    } 
    else if (edit_mode) {
      // PARAMETER EINSTELLEN
      if (current_domain == DOMAIN_LIGHT && selected == 1) param_value = std::max(1, std::min(100, param_value + (step*5))); // Helligkeit 1-100%
      else if (current_domain == DOMAIN_LIGHT && selected == 2) { param_value += (step*15); if(param_value>360) param_value=0; if(param_value<0) param_value=360; } // Farbe 0-360
      else if (current_domain == DOMAIN_CLIMATE && selected == 0) param_value = std::max(16, std::min(30, param_value + step)); // Temp 16-30°C
      else if (current_domain == DOMAIN_FAN && selected == 1) param_value = std::max(0, std::min(100, param_value + (step*10))); // Fan Speed
    }
  }

  void on_button() override {
    if (level == 0) { level = 1; selected = 0; offset = 0; }
    else if (level == 1) { 
      if (!rooms.empty() && on_room_selected) {
        std::string raum = rooms[selected];
        std::transform(raum.begin(), raum.end(), raum.begin(), ::tolower);
        on_room_selected(raum);
      }
    }
    else if (level == 2) {
      if (!devices.empty() && devices[0] != "Keine Geräte") {
        std::string raw_dev = devices[selected];
        size_t hash_pos = raw_dev.find('#');
        if (hash_pos != std::string::npos) {
          entity = raw_dev.substr(0, hash_pos);
          
          // GERÄTETYP ANALYSIEREN
          if (entity.find("light.") == 0) { current_domain = DOMAIN_LIGHT; param_value = 50; } // Startwerte
          else if (entity.find("climate.") == 0) { current_domain = DOMAIN_CLIMATE; param_value = 21; }
          else if (entity.find("fan.") == 0) { current_domain = DOMAIN_FAN; param_value = 50; }
          else current_domain = DOMAIN_DEFAULT;

          level = 3; selected = 0; offset = 0; edit_mode = false;
        }
      }
    }
    else if (level == 3) {
      // IST ES EIN KNOPF ODER EIN SCHIEBEREGLER?
      bool is_slider = false;
      std::string action_str = "TOGGLE";

      if (current_domain == DOMAIN_LIGHT) {
          if (selected == 0) action_str = "TOGGLE";
          else if (selected == 1) { is_slider = true; action_str = "BRIGHTNESS"; }
          else if (selected == 2) { is_slider = true; action_str = "COLOR"; }
      } 
      else if (current_domain == DOMAIN_CLIMATE) {
          if (selected == 0) { is_slider = true; action_str = "TEMP"; }
          else if (selected == 1) action_str = "TURN_ON";
          else if (selected == 2) action_str = "TURN_OFF";
      }
      else if (current_domain == DOMAIN_FAN) {
          if (selected == 0) action_str = "TOGGLE";
          else if (selected == 1) { is_slider = true; action_str = "SPEED"; }
      }
      else {
          if (selected == 0) action_str = "TOGGLE";
          else if (selected == 1) action_str = "TURN_ON";
          else if (selected == 2) action_str = "TURN_OFF";
      }

      // AKTION AUSFÜHREN
      if (is_slider) {
          if (!edit_mode) edit_mode = true; // In den Einstell-Modus wechseln
          else {
              edit_mode = false; // Wert bestätigen und abschicken
              if (on_advanced_action) on_advanced_action(entity, action_str, param_value);
          }
      } else {
          // Direkter Button-Klick (z.B. Toggle)
          if (on_advanced_action) on_advanced_action(entity, action_str, 0);
      }
    }
  }

  void go_back() {
    if (edit_mode) edit_mode = false;
    else if (level > 0) { level--; selected = 0; offset = 0; }
  }
};