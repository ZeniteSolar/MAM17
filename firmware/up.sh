make clean all
fuser -k /dev/ttyACM0
avrdude -c arduino -P /dev/ttyACM0 -p m328p -D -U flash:w:bin/firmware.elf
