## 🌟 Key Features
Zero YAML Spaghetti: The UI is completely modularized into C++ classes (State Pattern). Adding a new page or logic is extremely simple and keeps the YAML under 200 lines.

Dynamic Home Assistant Control: The device automatically detects your HA entities.

Lights: Shows Brightness (%) and Color (Hue) sliders.

Climate: Turns the dial into a thermostat (°C).

Fans: Adjusts speed percentages.

"Two-Factor" Security System: * Global Lockscreen: The display wakes up to show time/weather, but requires a 4-digit PIN to access any controls.

Admin Menu: Hidden menu to change the display PIN, set the display timeout (30-180s), or toggle a debug mode.

Emergency Fallback: An 8-digit hardcoded fallback sequence in case you forget your PINs.

Power-Loss Safe: All custom settings (PINs, Timeout) are saved directly to the ESP32's flash memory (NVS) and survive reboots instantly.

Tactile & Audio Feedback: Every rotation step and button press is synced with the internal buzzer for a premium, mechanical feel.

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
````
## 🚀 Getting Started
I've made the setup as easy as possible. You don't need to touch the C++ code to get started.

Clone the repository: [Link to your GitHub repo]

Copy the m5dial folder into your ESPHome configuration directory.

Add your passwords and default PINs to your secrets.yaml.

Flash the m5-stack-dial.yaml to your device!

All the UI graphics (Neon Style) are included in the repo.

I’d love to hear your feedback! The next step on the roadmap (V0.9.0) is building out a full Media Player page. If anyone wants to contribute or fork it, feel free!

Cheers!
