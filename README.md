# CD-i Controller Library
> Arduino Library for CD-i controllers
The code is released under the GNU General Public License.

## Summary
This library can be used to communicate as a controller to a CD-i player. It uses the SoftwareSerial library for serial communication.

## Changelog
Version | Date       | Comments
------- | ---------- | ---------------
v1.0.0  | 03-01-2021 | Initial Release
v1.0.1  | 06-01-2021 | Added experimental keyboard support, added and changed some examples

## Table of contents
* [Installation](#installation)
* [Libary Usage](#library-usage)
	* [Hardware Setup](#hardware-setup)
	* [Software Usage](#software-usage)
	* [Devices](#devices)
	* [Input Modes](#input-modes)
* [Examples](#examples)
* [Known Issues](#known-issues)
* [ToDo](#todo)

## Installation
For installation see: <https://www.arduino.cc/en/Guide/Libraries#importing-a-zip-library> or simply place the CdiController.cpp and CdiController.h files in the sketch folder.

## Library Usage

### Hardware Setup
In order to communicate with the CD-i player, the controller port pins "RTS" (pin 7) and "RXD" (pin 2) need to be connected to a digital input on the Arduino board.

### Software Usage
In your sketch you declare a controller using `CdiController Cdi(<RTS_PIN>, <PIN_RXD>, <DEVICE_MODE>);`, where you pass which pins are connected to the CD-i controller port RTS and RXD pins, as well as the device type.

The library itself is initialised by calling the `CdiController.Init()` function (preferable in your sketch' `setup()` section. Within the `loop()` section of your sketch you need to call the `CdiController.Task()` function which handles the connection and identification to the CD-i player.

### Devices
The CD-i player supports different kind of input devices. Four modes are supported by the library, passing in the correct mode to the library during declaration:
* **Relative Coordinate Devices** - Device Mode `RELATIVE`: For example Mice, Trackballs
* **Maneuvering Devices** - Device Mode `MANUEVER`: For example Joysticks, Joypads
* **Absolute Coordinate Devices** - Device Mode `ABSOLUTE`: For example Graphics Tablets, Light Pens
* **Absolute Screen Devices** - Device Mode `SCREEN`: For example Touch Screens, Light Pens
* **Keyboard - Device Mode** `KEYBOARD`: Simple keyboard
* **Extended Keyboard** - `EXT_KEYB`: Full 104 key keyboard

### Input Modes
The libary provides the following functions for transmitting data to the CD-i player:

* `JoyInput(x, y, button_1, button_2)` For RELATIVE and MANEUVER modes. X and Y specify the relative movement since the last update, values can be between -128 and 127. Button 1 and Button 2 are boolean values indicating if these buttons are depressed or not.
* `PenInput(x, y, button_1, button_2, pen_down)` For ABSOLUTE and SCREEN modes. X and Y specify the absolute location of the cursor on screen, going from 0 (Outermost left, top of the screen) to 1023 (Outermost right, bottom of the screen). Button 1, Button 2 and Pen Down are boolean values indicating if these buttons are depressed or not.

For all of these functions a boolean value is returned indicating if data has actually been sent to the CD-i player. 

## Examples
Most of the examples make use of an USB Host Shield (<https://chome.nerpa.tech/arduino_usb_host_shield_projects/>) to use a controller as input, then converting this to a CD-i joystick. The following examples are included:

* **AnalogStick**: Uses an analog stick connected to the analog inputs of the Arduino as input for a CD-i Maneuvering Device. Clicking the stick serves as button 1.
* **MagicNS**: Uses a Switch Pro controller connected to a Magic NS adapter, connected to a USB Host Shield as input for a CD-i Maneuvering Device. Both the left stick and d-pad are mapped to movement. With the left/right triggers a speed can be selected.
* **USBKeyMouse**: Uses a Keyboard and/or Mouse connected to the USB port of a USB Host Shield as a CD-i Relative Coordinate Device. The mouse is mapped as expected, for a keyboard the directional keys are mapped to movement, 'Z' is mapped to button 1 and 'X' is mapped to button 2. The number keys (0-9) can be used for speed selection.
* **WiiController**: Uses a WiiMote or Wii U Pro controller connected to Bluetooth adapter, connected to a USB Host Shield as input for a CD-i Maneuvering Device. Both the left stick and d-pad are mapped to movement. With the plus/minus buttons a speed can be selected. It assumes a sideways orientation for a WiiMote (D-pad left, buttons 1 and 2 to the right).
* **WiiLightGun**: Uses a WiiMote connected to a Bluetooth adapter, connected to a USB Host Shield as input for a CD-i Absolute Coordinate Device. It uses the IR mode of the WiiMote, which needs to be enabled in the 'settings.h' file for in the USB Host Shield Library. Use button '1' and '2' to enable and disable IR Mode. Button A and B on the WiiMote are mapped to buttons 1 and 2 on the CD-i.
* **WiiUProAbsolute**: Uses a Wii U Pro controller connected to Bluetooth adapter, connected to a USB Host Shield as input for a CD-i Absolute Coordinate Device. The range of the left stick is mapped to the full screen, the right stick provides a smaller offset for fine tuning.

## Known Issues
A simulated controller may not function as expected in all games. For example using the Joypad examples with "The Apprentice" game, the running animation keeps restarting and also the maximum running speed is never reached. This is due to rate limiting of the packets to the CD-i player. Lowering this delay (20ms in the example) introduces lag in the controller, as the USB buffer fills up faster than data can be sent to the CD-i player.

## ToDo
* Document and test CD-i Keyboard support