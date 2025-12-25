# 🦖 8051 Dino Game: Dual-Prototype Implementation

> **A retro-style implementation of the Google Chrome "Dino Runner," engineered from scratch for the 8051 Microcontroller architecture.**


## 📄 Project Overview
This project is a hardware-level gaming console built on the **AT89C51**, a classic 8-bit microcontroller. Unlike modern game development that relies on operating systems and graphics engines, this project runs on "bare metal", managing every pixel, clock cycle, and memory address manually.

We built two distinct prototypes to demonstrate versatile hardware interfacing:
1.  **Bare-Chip LCD System:** Using a raw DIP-40 IC and a character display.
2.  **OLED Dev-Kit System:** Using a development board with a high-resolution graphical display.

## 🧐 The Problem
Developing a real-time game on 1980s architecture presents significant engineering constraints:
* **Extreme Memory Limits:** The 8051 has only **128 bytes of RAM** and 4KB of Flash. Storing graphics and game state is difficult.
* **No Native Drivers:** There are no built-in libraries for modern displays like OLEDs or complex LCD timing.
* **Timing Sensitivity:** A single microsecond delay error can cause the display to fail initialization or the game to stutter.

## 💡 The Solution
We engineered a custom firmware solution that maximizes the 8051's potential:
* **Dual-Display Engine:** We wrote two separate drivers—one for **4-bit Parallel communication** (LCD) and another for **Bit-Banged I2C** (OLED).
* **Memory Optimization:** We bypassed the RAM limit by storing all sprite bitmaps (Dino, Cactus, Fonts) directly in the Flash (`CODE`) memory.
* **Flicker-Free Logic:** Instead of clearing the screen every frame (which causes flickering), our rendering engine only updates the specific pixels that changed position.

## 🛠️ Tech Stack
* **Microcontroller:** AT89C51 (8051 Architecture)
* **Language:** Embedded C (Keil C51)
* **Displays:** JHD204A (20x4 LCD), SSD1306 (0.96" OLED)
* **Hardware Tools:** USBASP Programmer, Breadboards, Tactile Switches
* **Simulation:** Proteus 8 Professional

## ✨ Key Features
* 🏃 **Dual Game Modes:** Includes "Classic Dino Run" and a "Terrain Challenge" variant.
* 👾 **Custom Graphics:** Manually designed 5x8 pixel sprites (LCD) and 8x8 bitmaps (OLED).
* 🔊 **Audio Feedback:** Active buzzer provides real-time sound effects for jumps and collisions.
* ⚡ **Optimized Controls:** Responsive input handling using active-low polling logic.

## 🔌 Circuit Diagram / Pinout
To replicate this project, connect the components as follows:

| Component | Pin Name | Connected to 8051 Port |
| :--- | :--- | :--- |
| **LCD Data** | D4 - D7 | **P2.4 - P2.7** |
| **LCD Control** | RS | **P3.5** |
| **LCD Control** | EN | **P3.7** |
| **OLED** | SCL / SDA | **P2.1 / P2.2** |
| **Input** | Jump / Start | **P1.0 / P1.3** |
| **Output** | Buzzer | **P1.1** |

## 🚧 Engineering Challenges & Solutions

### 1. The "Black Box" LCD Failure
* **Challenge:** In early tests, the LCD would remain blank or show black boxes despite correct wiring.
* **Solution:** We analyzed the HD44780 datasheet and discovered our Enable (EN) pulse was too slow (millisecond range). We wrote a custom `delay_us(5)` function to provide the precise 5-microsecond pulse required for initialization.

### 2. Screen Flicker on OLED
* **Challenge:** The OLED version flickered uncontrollably because we were clearing the entire 1024-byte screen buffer every frame over a slow software I2C bus.
* **Solution:** We implemented a **"Dirty Rectangle" algorithm**. The code now tracks the Dino's old position and only erases that specific 8x8 block before drawing the new one, reducing data transfer by 95%.

### 3. RAM Overflow
* **Challenge:** Defining arrays for game graphics immediately crashed the stack because the 8051 only has 128 bytes of RAM.
* **Solution:** We used the proprietary `code` keyword (e.g., `unsigned char code dino[]`) to force the compiler to store constant data in the Read-Only Memory (Flash), freeing up RAM for game variables.

## 🚀 How to Run

### Software Implementation
1.  **Clone the Repository**
    ```bash
    git clone [https://github.com/dassm309-sys/8051-Dino-Game.git](https://github.com/dassm309-sys/8051-Dino-Game.git)
    ```
2.  Open the project in **Keil µVision**.
3.  Select the **AT89C51** target and set the crystal frequency to **12.0 MHz**.
4.  Build the Target to generate the `.hex` file.

### Hardware Implementation
1.  Use a **USB ISP Programmer** (e.g., USBasp) to burn the generated `.hex` file onto the AT89C51 chip.
2.  Wire the breadboard according to the **Pinout table** above or the schematic in `/assets`.
3.  Power the circuit with a **5V source**.

## 👥 Authors

| Name | Role | ID |
| :--- | :--- | :--- |
| **Midhun Dhass D** | Firmware Engineering & Optimization | 24BLC1037 |
| **Linga Raja R** | Hardware Design & Prototyping | 24BVD1110 |

---
*Created at VIT Chennai.*