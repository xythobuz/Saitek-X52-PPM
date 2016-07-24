# Arduino Saitek X52 USB CPPM converter

This sketch allows connecting a [Saitek X52](http://www.saitek.com/uk/prod/x52.html) or [Saitek X52 Pro](http://www.saitek.com/uk/prod/x52pro.html) to an [Arduino](https://www.arduino.cc/en/Main/ArduinoBoardUno) with a [USB Host Shield](https://www.arduino.cc/en/Main/ArduinoUSBHostShield). It uses the [USB_Host_Shield_2.0 Library](https://github.com/felis/USB_Host_Shield_2.0).

A CPPM signal is generated on a configurable pin and can be fed into a transmitter module or directly into a flight controller.

Using the Multi-Function-Display on the Joystick, every config option can be changed. These values can be stored on the EEPROM and will be loaded on every start.

I'm connecting the Arduino to the [FrSky DHT module](http://www.frsky-rc.com/product/pro.php?pro_id=7) in my modified transmitter to control my Tricopter.

A modified (ie. non-inverted) FrSky Host Telemetry Port (D-Port) can be connected to the hardware serial port of the Arduino so the Telemetry data (link quality and voltages) will be displayed on the Multi-Function-Display of the joystick.

## License

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.

