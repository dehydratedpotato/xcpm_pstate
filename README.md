# xcpm_pstate
Reading `/dev/xcpm` pstate counters to calculate requested package frequency on Intel Macs by infering some structs and doing other crap. The xcpm stands for XNU CPU Power Management, which is Apple's power manager for Intel Macs with Haswell (and newer) chips.

I figured this method out while reverse engineering [`powermetrics`](https://www.unix.com/man-page/osx/1/powermetrics/), which uses the xcpm to provide data for the `--show-pstates` flag.

This method sadly reuires root in order to make ioctl calls to /dev :(

## Building
Use whatever compiler you like, such as `gcc`, for example: `gcc ./xcpm_pstate.c -o xcpm_pstate`

## Example Output
Example running on MacBookPro11,1. Default sampling rate is 1s.
```
Max Efficient State: 3000 MHz
Non-Turbo States: 800-3000 MHz
Turbo States: 800-3500 MHz

Requested Distribution: 3500MHz: 3.86%  3400MHz: 0.00%  3300MHz: 0.00%  3200MHz: 5.75%  3100MHz: 0.00%  3000MHz: 31.20%  2900MHz: 0.00%  2800MHz: 0.00%  2700MHz: 0.00%  2600MHz: 0.00%  2500MHz: 0.00%  2400MHz: 0.00%  2300MHz: 0.00%  2200MHz: 0.00%  2100MHz: 0.00%  2000MHz: 0.00%  1900MHz: 52.27%  1800MHz: 0.00%  1700MHz: 0.00%  1600MHz: 0.00%  1500MHz: 0.00%  1400MHz: 0.00%  1300MHz: 0.00%  1200MHz: 0.00%  1100MHz: 0.00%  1000MHz: 0.00%  900MHz: 0.00%  800MHz: 6.91%  

Requested Package Frequency: 2303.75 MHz
```
