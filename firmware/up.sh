make clean
rm -rf bin obj
make all
#fuser -k /dev/ttyUSB0
#sudo chown $(whoami) /dev/ttyUSB0
avrdude -c arduino -P /dev/ttyUSB0 -p m328p -D -U flash:w:bin/firmware.elf
figlet "MAM17"
