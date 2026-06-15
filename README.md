>  **LEGAL NOTICE: PATENT PENDING** > The system architecture, sensor fusion methodology (Crowd Instability Index), and adaptive thresholding algorithms detailed in this repository are subject to an active, pending patent application. This code is provided strictly for academic and professional portfolio review. Commercial use, reproduction, or distribution without explicit permission is strictly prohibited.
# LoRa-Based Distributed Sensor Network for Real-Time Crowd Instability Detection

A decentralized, internet-independent hardware infrastructure designed to monitor and predict localized crowd surges and stampede risks at high-density public gatherings. 

This project utilizes edge-computing sensor fusion (accelerometers, load cells, and acoustic modules) combined with statistical adaptive thresholding to detect dangerous crowd compression dynamics before catastrophic failures occur.

##  Technical Stack
* **Microcontroller:** ESP32 Dual-Core (FreeRTOS compatible)
* **RF Communication:** Sub-GHz LoRa (SX1278 / 433MHz)
* **Sensors:** ADXL345 (I2C), HX711 (Serial), MAX9814 (ADC)
* **Firmware:** C++ (PlatformIO framework)
* **Signal Processing:** Non-blocking sliding-window filters, moving average/std-dev ($\mu + k\sigma$), and time-delta derivations ($d/dt$).

##  System Architecture

The architecture relies on a **Star Topology** where multiple Edge Nodes process physical telemetry locally to reduce over-the-air bandwidth, sending only tokenized risk packets to a central Gateway.

1. **The Edge Node:** Computes a custom **Crowd Instability Index (CII)** by fusing normalized data from pressure accumulation (Load Cell), kinetic ground vibration (Accelerometer), and acoustic panic spikes (Microphone).
2. **The RF Link:** Utilizes the LoRa spread spectrum modulation technique to penetrate dense human masses and concrete barriers, ensuring communication remains intact even when local cellular networks collapse during emergencies.
3. **The Gateway Server:** Parses incoming CSV telemetry from the node network and evaluates emergency conditions based on adaptive statistical bounds, filtering out environmental noise floors.

##  Repository Structure
* `/firmware`: Contains the PlatformIO project. Uses build environments to separate Node and Gateway C++ source code.
* `/hardware`: Includes the Bill of Materials (BOM) and pinout/wiring tables for all I2C, SPI, and Analog bus connections.
* `/docs`: Contains the formal project report and research background.

##  Getting Started

This repository is configured for **PlatformIO**. 

1. Clone the repository and open the folder in VS Code with the PlatformIO extension installed.
2. The `platformio.ini` file handles all library dependencies automatically (e.g., `sandeepmistry/LoRa`, `bogde/HX711`).
3. To flash the sensor hardware, select the `env:transmitter_node` build target.
4. To flash the receiving station, select the `env:receiver_server` build target.

##  Future Roadmap (Full-Stack Integration)
* **PWA Security Dashboard:** Develop a React-based frontend mapping interface for security personnel to visualize node telemetry.
* **Database Backend:** Pipe Gateway serial data via a local Node.js service into a PostgreSQL database (Supabase) for long-term historical analysis of crowd compression trends.
* **Mesh Protocol:** Upgrade from a Star topology to a multi-hop mesh network to extend range in highly obstructed environments.
