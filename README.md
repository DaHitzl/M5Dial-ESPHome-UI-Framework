# 🎛️ M5Stack Dial - Modular ESPHome UI Framework

A blazing fast, object-oriented, and event-driven C++ UI framework for the M5Stack Dial in ESPHome. 

Tired of scrolling through thousands of lines of YAML to change a simple menu? This project completely separates the **Hardware Configuration (YAML)** from the **UI Logic (C++)**.

## ✨ Features
* **Zero YAML UI Logic:** The `.yaml` file only defines hardware pins and Home Assistant services. All UI drawing and navigation is handled by a highly optimized C++ class.
* **Blazing Fast Response:** Intelligent asynchronous updates and `wait_until` triggers ensure instant display feedback, even while RTTTL sounds are playing.
* **Modular Page System (State Pattern):** Each page (Home Assistant, Settings, Music) is a completely isolated `.h` file. They communicate with the main controller via clean `std::function` callbacks.
* **Plug & Play:** Easily add new pages (like a Media Player or RGB Color Picker) without touching the main logic.

## 📂 Folder Structure
Keep your ESPHome directory completely clean:
```text
/config/esphome/
  ├── m5-stack-dial.yaml        <-- Hardware & Home Assistant API ONLY
  └── m5dial/                   <-- The C++ Brain
       ├── m5dial_ui.h          <-- Main Controller
       ├── page_interface.h     <-- Base Class for all pages
       ├── page_main.h          
       ├── page_ha.h            <-- Isolated HA logic
       ├── page_settings.h      
       ├── ui_helpers.h         <-- Dumb drawing functions
       └── sound_manager.h      <-- Centralized RTTTL manager

🚀 Installation
Copy the m5dial folder and the m5-stack-dial.yaml into your ESPHome configuration directory.

Open m5-stack-dial.yaml and adjust:

Your Wi-Fi credentials (!secret)

Your specific Home Assistant sensors (e.g., Shelly temperature entities).

Click "Install" in your ESPHome dashboard.

🛠️ How to add a new Page
Adding a new page is incredibly simple due to the decoupled architecture:

Create a new file page_custom.h that inherits from PageInterface.

Implement your draw(), on_encoder(), and on_touch() logic.

Include it in m5dial_ui.h, add it to the state machine, and map your callbacks. Done!
