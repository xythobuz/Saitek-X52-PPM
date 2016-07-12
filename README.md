# Arduino Saitek X52 USB CPPM converter

This sketch allows connecting a [Saitek X52](http://www.saitek.com/uk/prod/x52.html) or [Saitek X52 Pro](http://www.saitek.com/uk/prod/x52pro.html) to an [Arduino](https://www.arduino.cc/en/Main/ArduinoBoardUno) with a [USB Host Shield](https://www.arduino.cc/en/Main/ArduinoUSBHostShield). It uses the [USB_Host_Shield_2.0 Library](https://github.com/felis/USB_Host_Shield_2.0).

A CPPM signal is generated on a configurable pin (4 by default, can be changed in `cppm.h`).

It includes code to interface with the Multi-Function-Display on the Joystick to display text and change the LED and background lighting.

I'm connecting the Arduino to the [FrSky DHT module](http://www.frsky-rc.com/product/pro.php?pro_id=7) in my modified transmitter to control my Tricopter.

## License

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.

