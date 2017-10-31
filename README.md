# MIDI_Footswitchbar
_Is a firmware for Arduino UNO to use switches connected to GPIOs to trigger midi notes. It uses arduino_midi_library and requires hiduino firmware to be flashed onto atmega8u2 or atmega16u2 microcontroller, replacing its original usbserial firmware, to act as usb hid midi device._

## Example steps to get it up and running
* clone MIDI_Footswitchbar from https://github.com/ChristianAhrens/MIDI_Footswitchbar
* clone hiduino from https://github.com/ddiakopoulos/hiduino
* hiduino contains precompiled hex firmwares to be flashed e.g. by utilizing DFU mode of atmega microcontrollers (weird enough, the board this has been tested with has a atmega16u2 as usb serial driver chip - yet only the usbserial_uno_8u2.hex firmware does the job correctly)
	*	usbserial_mega_8u2.hex to enable Arduino IDE to interact with arduino and flash the MIDI_Footswitchbar firmware
		1.	reset board by grounding its reset pin
		2.	*sudo dfu-programmer atmega16u2 erase*
		3.	*sudo dfu-programmer atmega16u2 flash usbserial_uno_8u2.hex*
		4.	*sudo dfu-programmer atmega16u2 reset*
		5.	use Arduino IDE as usual
	*	arduino_midi.hex to have the thing act as MIDI HID device
		1.	reset board by grounding its reset pin
		2.	*sudo dfu-programmer atmega16u2 erase*
		3.	*sudo dfu-programmer atmega16u2 flash arduino_midi.hex*
		4.	*sudo dfu-programmer atmega16u2 reset*
		5.	use board as MIDI USB HID device