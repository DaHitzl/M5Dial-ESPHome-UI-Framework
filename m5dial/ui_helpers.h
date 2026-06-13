#pragma once
#include "esphome.h"
#include <vector>
#include <algorithm>

using namespace esphome;
using namespace esphome::display;

class UIHelpers {
public:
  static void draw_header(Display &it, const char *titel, Color farbe) {
    it.print(120, 30, &id(meine_schrift), farbe, TextAlign::CENTER, titel);
  }

  static void draw_menu(Display &it, const std::vector<std::string> &eintraege, int selected, int offset, int y_start, int zeilenhoehe) {
    int ende = std::min(offset + 3, (int)eintraege.size());
    int y = y_start;
    for (int i = offset; i < ende; i++) {
      Color c = (i == selected) ? Color(255, 255, 0) : Color(180, 180, 180);
      if (i == selected) it.print(35, y, &id(meine_schrift), c, TextAlign::LEFT, ">");
      it.print(55, y, &id(meine_schrift), c, TextAlign::LEFT, eintraege[i].c_str());
      y += zeilenhoehe;
    }
  }

  static void move_selection(int step, int &selected, int &offset, int total_items, int visible_items = 3) {
    int new_sel = selected + step;
    if (new_sel >= 0 && new_sel < total_items) {
      selected = new_sel;
      if (selected < offset) offset = selected;
      else if (selected >= offset + visible_items) offset = selected - visible_items + 1;
    }
  }
};