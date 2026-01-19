
### Auto_Gate

Auto_Gate is a project for an automatic gate control system, the aim of which is to implement a safe and reliable process of opening and closing the gate using the ESP32-WROOM-32 microcontroller.
The project focuses on a clear code structure, modularity, and the possibility of further expansion with additional features, such as remote control or integration with smart home systems.
The source code is located in the src/ directory and implements gate control logic, input/output handling, and basic decision-making mechanisms responsible for the correct operation of the system. An example is the ability to open the gate using a remote control, WIFI, or mqtt server.
The project can be used both as a basis for actual hardware implementation and as a demonstration of embedded systems programming skills.
The solution was designed for:
- home automation,
- embedded systems

### Project Structure

src/
├── main.cpp              # Application entry point
├── gate_control.*        # Gate control logic
├── Actuator.*            # Motor driver handling
├── CurrentProtection.*   # Current and voltage measurement
├── Network.*             # MQTT server control
├── PCA.*                 # PWM signal
├── RF433.*               # (pilot) radio control
├── WebServerModule.*.    # WiFi control


---

## Technology

- ESP32-WROOM-32  
- PlatformIO  
- C++  

### Used Components
- PCA9685  
- 2x BTS7960  
- 2x INA226  
- 433 MHz receiver  
- 2x Actuator 24V, 3.5A, 1500N, IP44  

---

## Features

- Automatic opening and closing of the gate  
- Control via:
  - 433 MHz remote control  
  - Wi-Fi  
  - MQTT server  
- Motor control using BTS7960 drivers  
- Current and voltage monitoring using INA226  
- PWM control via PCA9685  
- Modular and extensible architecture  

---

## System Architecture

The system is based on the **ESP32-WROOM-32 microcontroller**, which acts as the main control unit.  
It communicates with peripheral modules responsible for motor control, power monitoring, and external inputs.

**Main responsibilities:**
- ESP32: decision logic, communication, system control  
- PCA9685: PWM signal generation  
- BTS7960: motor driver for gate actuators  
- INA226: current and voltage measurement  
- 433 MHz receiver: remote control input  
- Wi-Fi / MQTT: remote and smart home integration  

---

## Future Improvements

- Web-based configuration interface  
- Mobile application integration  
- Endstop and safety sensor support  
- OTA firmware updates  
- Logging and diagnostics via MQTT  

---

## Author

- Arkadiusz  
- GitHub: [https://github.com/Arkadius0](https://github.com/Arkadius0)  

---

## License

This project is licensed under the **Apache License 2.0**.  
For details, see the [LICENSE](LICENSE) file.
