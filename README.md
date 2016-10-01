# Infrared on Arduino101

A project to enable sending raw codes over infrared which supports
the [Arduino101](https://www.arduino.cc/en/Main/ArduinoBoard101) 
development board.

This project utilizes the [Lirc101](https://github.com/dantler/LircRemote101) library by [dantler](https://github.com/dantler).

Raw codes must be provided in the form of an array in the format
used by [lirc](http://lirc.org/html/lircd.conf.html) under
"begin raw_codes".  Currently a 38KHz signal is the only supported
output frequency.  See the example file in cf_files/kanto_yu5.cf.

# License

BSD license.

