# Projector IR Controller

An Arduino-based infrared (IR) controller for the Dangbei N2 Smart Projector that automates power management and input selection sequences.

## Overview

This project addresses the inconvenience of managing multiple remotes in a home theater setup. The controller automates the process of turning the projector on/off and configuring it to the correct input device, eliminating the need for a separate projector remote.

## Problem Statement

In a typical setup with a Dangbei N2 Smart Projector, FireTV Stick, and FireTV Soundbar, users need multiple remotes:

- FireTV remote for FireTV Stick and soundbar control
- Projector remote for power management and input selection - it's not possible to use the FireTV remote for this because the projector remote has a different protocol (more info: https://www.reddit.com/r/projectors/comments/15r04dq/dangbei_ir_remote_code/)

This project consolidates projector control into a simple physical switch, automating common sequences like power on/off and HDMI input selection.

## Hardware Components

### Circuit Design

The project includes a custom Arduino circuit with:

- **Arduino Board**: Main microcontroller
- **TSOP38238 IR Receiver**: For recording IR codes from the original remote
- **950nm IR LED**: For sending IR signals to the projector
- **Physical Switch**: Manual trigger for power sequences
- **Status LED**: Visual feedback for operation status

### Circuit Documentation

The schematic was built using Circuit Canvas and can be found [here](https://circuitcanvas.com/p/x1f8y9dr6xvdkji8kt7?canvas=schematic).

#### Schematic

![Detailed schematic](img/Projector%20IR%20Controller-schematic.png)

#### Layout

![Circuit layout diagram](img/Projector%20IR%20Controller-layout.png)

## Software Implementation

### Dependencies

- [Arduino IR Remote Library](https://github.com/Arduino-IRremote/Arduino-IRremote), version 4.4.3 - For IR signal transmission and reception

### Key Features

- **IR Code Recording**: Built-in recording mode using the Arduino IR Remote Library to capture all remote button codes
- **Sequenced Operations**: Implements sequence handling for power management and input selection
- **Physical Interface**: Simple switch-based control with LED status indicators

### Functionality

#### Recording mode

To record IR codes from the original remote:

1. Uncomment the line `#define OPERATING_MODE_RECORDING` at the top of the Arduino sketch
2. Upload the code to the Arduino
3. Point the original remote at the TSOP38238 IR receiver
4. Press buttons on the remote to record their IR codes
5. The codes will be printed to the Serial Monitor in the format:
   ```
   Protocol=NEC Address=0x281 Command=0x1 Raw-Data=0xFE010281 32 bits LSB first
   Send with: IrSender.sendNEC(0x281, 0x1, <numberOfRepeats>);
   ```
6. Comment out the `#define OPERATING_MODE_RECORDING` line to return to normal operation

#### Normal mode

1. Power Off Sequence

   1. Sends POWER command
   2. Sends OK command to confirm shutdown dialog

2. Power On Sequence

   1. Sends POWER command
   2. Waits 10 seconds for device startup
   3. Navigates to HOME menu
   4. Selects HDMI input via UP/DOWN navigation
   5. Confirms selection with OK

## IR Codes

The project includes comprehensive IR code mapping for the Dangbei N2 Smart Projector:

| Button      | Protocol | Address | Command | Raw Data   |
| ----------- | -------- | ------- | ------- | ---------- |
| POWER       | NEC      | 0x281   | 0x1     | 0xFE010281 |
| MUTE        | NEC      | 0x281   | 0x10    | 0xEF100281 |
| OK          | NEC      | 0x281   | 0x5     | 0xFA050281 |
| UP          | NEC      | 0x281   | 0x3     | 0xFC030281 |
| RIGHT       | NEC      | 0x281   | 0x6     | 0xF9060281 |
| LEFT        | NEC      | 0x281   | 0x4     | 0xFB040281 |
| DOWN        | NEC      | 0x281   | 0x7     | 0xF8070281 |
| HOME        | NEC      | 0x281   | 0x9     | 0xF6090281 |
| BACK        | NEC      | 0x281   | 0x8     | 0xF7080281 |
| VOLUME UP   | NEC      | 0x281   | 0xB     | 0xF40B0281 |
| VOLUME DOWN | NEC      | 0x281   | 0xC     | 0xF30C0281 |
| AUTOFOCUS   | NEC      | 0x281   | 0x14    | 0xEB140281 |
| NETFLIX     | NEC      | 0x281   | 0x58    | 0xA7580281 |
| SETTINGS    | NEC      | 0x281   | 0xA     | 0xF50A0281 |
| PRIME VIDEO | NEC      | 0x281   | 0x50    | 0xAF500281 |
| YOUTUBE     | NEC      | 0x281   | 0x55    | 0xAA550281 |

## Usage

1. **Setup**: Connect the circuit to your Arduino and upload the `src/ProjectorController/ProjectorController.ino` code
2. **Operation**: Flip the physical switch to trigger power on/off sequences
3. **Status**: LED indicators provide visual feedback during operation

## Technical Details

- **IR Protocol**: NEC protocol with 32-bit data transmission
- **Frequency**: Standard IR frequency (38kHz)
- **Repeats**: 5 repeat transmissions for reliable signal reception
- **Timing**: Optimized delays for device response times

## Caveats

- The projector startup sometimes takes shorter, sometimes takes longer. It'd be reasonable to add a light sensor and place it close to the projector lamp to detect when the projector is on.
