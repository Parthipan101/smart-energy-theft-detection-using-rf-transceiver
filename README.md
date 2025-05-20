# Smart Energy Theft Detection System

This project implements a real-time energy theft detection system using Arduino, voltage/current sensors, and wireless communication via nRF24L01. It compares electrical data between two points—supply (transmitter) and consumer (receiver)—to detect and flag unauthorized energy usage.

## 🔧 Project Description

The system is designed to address the growing issue of electricity theft by:

- Monitoring voltage and current on both ends of a distribution line.
- Sending data wirelessly from transmitter to receiver.
- Comparing values at the receiver to detect mismatches.
- Displaying energy parameters and alerts on an LCD.
- Supporting expansion through IoT, GSM, or cloud platforms.

## 🧠 Features

- Real-time power and energy monitoring
- Voltage and current measurement using analog sensors
- Wireless communication using nRF24L01 modules
- Energy theft detection based on mismatch analysis
- LCD display for live data feedback
- Serial output for debugging and logging

## 🧱 Hardware Used

- Arduino Nano (or Uno)
- nRF24L01 Transceiver Modules (2 units)
- Voltage Sensor (e.g., ZMPT101B)
- Current Sensor (e.g., ACS712)
- I2C LCD Display (16x2)
- Resistors, Breadboard, Jumper Wires
- Power Supply

## 💻 Software Used

- Arduino IDE
- C/C++ (Arduino sketches)
- Serial Monitor for output visualization

## 🚀 How It Works

1. The **transmitter** reads voltage and current, then sends data wirelessly.
2. The **receiver** also reads its own voltage and current values.
3. It compares the received data with local readings.
4. If the mismatch exceeds the threshold, theft is flagged.
5. Data is displayed on the LCD and logged via serial.

## 📈 Future Enhancements

- Cloud and IoT-based remote monitoring
- GSM module for SMS alerts
- AI/ML for intelligent theft prediction
- Tamper-proof sensor enclosures
- Integration with billing systems

## 📃 License

This project is open-source and available under the [MIT License](LICENSE).

---

🔌 *Developed for academic purposes and scalable for real-world deployment.*
