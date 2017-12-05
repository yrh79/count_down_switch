# count_down_switch
A count down switch implemented with Arduino Uno

This is an implement of the count down switch with Arduino Uno.

People may have the needs to restart a WIFI AP everyday just to make sure it works with no problem, but maybe that AP does not have the auto-reboot functionalities. Power cycle the AP may be a good way to keep it works perperly.

This count down switch could switch off then back on the power suply of the power consumer (WIFI AP) on a hardware pre-configured basic. A user could use a set of jumppers (GPIO 4 through 9) to quickly setup a cycle value:


quick setup:

pin 4 - 1 hour,

pin 5 - 2 hours,

pin 6 - 4 hours,

pin 7 - 6 hours,

pin 8 - 12 hours,

pin 9 - 48 hours,


All-open - 24 hours (default value)


bit-setup: 1 bit equals to 30 minutes, e.g.:

0x11 = 1.5 hours (jumpper set on pin 4 and 5)

0x101 - 2.5 hours (jumpper set on pin 4 and 6)


The switch would do a switch off-on action 11 hours after the initial power on - this is to allow people setup the power supply (e.g.) on 2pm in the afternoon, then the first (and every power cycle) action would happen at 1am in the mid-night.

Usage:

1. Connect Pin 2 to a relay's input and power it properly.

2. Clone https://github.com/JChristensen/Timer and place it into the libraries folder of your arduino IDE.

3. flash the Arduino Uno as usual then enjoy. :)

This project is tested under arduno 1.8.5. Other version of arduino IDE may also work.

