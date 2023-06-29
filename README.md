# source-clock
i lost the source code to this clock, so i made something better for it

## programming
```
# compile + flash + serial monitor
make DEV=/dev/ttyACM0 # or whatever
# compile + flash
make program DEV=/dev/ttyACM0
# compile
make build
# rebuild target
make clean
```
make sure to disconnect gps module from arduino before flashing or you will get sync errors
