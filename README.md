<div align="center">

🦖 8051 Based Dino Game 🎮

Dual-Prototype Implementation (LCD & OLED)

A retro-gaming engineering project built from scratch on the AT89C51 Microcontroller.

</div>

📖 Project Overview

This project is a hardware-level implementation of the classic Google Chrome Dino Runner, engineered to run on the resource-constrained 8051 architecture. Unlike modern game development, every pixel, clock cycle, and memory address here is managed manually.

Key Achievement: We successfully ported the game logic to two distinct display technologies—a character-based LCD and a graphical OLED—using custom-written software drivers.

👥 Authors

Name

Role

ID

Midhun Dhass D

Firmware Engineering & Optimization

24BLC1037

Linga Raja R

Hardware Design & Prototyping

24BVD1110

🛠️ System Architecture

We developed two independent prototypes to demonstrate versatile hardware interfacing:

🔰 Prototype 1: The "Bare Metal" LCD System

Built on a breadboard using the raw AT89C51 DIP-40 IC.

Display: JHD204A 20x4 Alphanumeric LCD.

Interface: 4-bit Parallel Mode (Optimized to save GPIO pins).

Visuals: Custom CGRAM Sprites (5x8 pixels) for the Dino and Cacti.

Key Challenge: Implementing a 5µs timing driver to fix LCD initialization failures.

⚡ Prototype 2: The OLED Dev-Kit System

Built using an 8051 Development Board for high-speed graphics.

Display: 0.96" SSD1306 OLED (128x64 Pixels).

Interface: Software Bit-Banged I2C Protocol (since 8051 has no native I2C).

Visuals: Pixel-perfect 8x8 Bitmaps stored in Flash (CODE) memory.

Key Challenge: Achieving a flicker-free frame rate by updating only moving pixels.

🎮 Game Features

🏃 Dual Game Modes: Select between Classic Dino Run and Terrain Challenge.

🚫 Flicker-Free Engine: Logic optimized to redraw only changed screen areas.

🔊 Audio Feedback: Active Buzzer alerts on jumps and collisions.

💾 Memory Optimization: Sprites stored in CODE memory to bypass the 128-byte RAM limit.

💻 Software Implementation Guide

1. Prerequisites

IDE: Keil µVision 5 (C51 Compiler).

Flasher: PROGISP or XGPRO (for USBASP/TL866 programmers).

2. How to Build & Flash

Open Project: Launch Keil and create a new project for the AT89C51.

Add Source: * For LCD: Add src/prototype-1-lcd/dino_game_4bit.c

For OLED: Add src/prototype-2-oled/dino_game_oled.c

Configure: Set Target Frequency to 12.0 MHz and check "Create HEX File".

Compile: Build the project (Ensure 0 Errors).

Flash: Upload the .hex file to your microcontroller.

📂 Repository Structure

/8051-Dino-Game
├── /src
│   ├── /prototype-1-lcd    # Source code for LCD version
│   └── /prototype-2-oled   # Source code for OLED version
├── /docs
│   ├── BOM.md              # Complete Bill of Materials
│   └── Technical_Manual.md # Deep dive into timing & logic
└── /assets                 # Schematics and Prototype photos


📝 License & References

License: Open Source (MIT).

References: Mazidi 8051 Textbook, SSD1306 Datasheet, HD44780 Datasheet.

Institution: VIT Chennai, School of Electronics Engineering (SENSE).