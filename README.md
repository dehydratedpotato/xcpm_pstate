# (WIP) xcpm_pstate
Reading `/dev/xcpm` pstate counters to calculate package frequency on Intel Macs. This method is derived from `powermetrics`. I found how to do this during my time reverse engineering that tool with ghidra.

I only have one Intel Mac so I can't confirm whether or not this trick working is just a coincidence; but I've run it a lot, plus looked at `powermetrics` too, and it seems solid for now.

<img width="434" alt="Screen Shot 2023-03-19 at 1 04 41 PM" src="https://user-images.githubusercontent.com/83843298/226205957-feaa9fd0-e243-432f-9b3d-6b699777b42b.png">

Currently, building this in the command line results in a segfault because...memcpy says so. Xcode gives us an output but ends with a sigbert. 


<img width="717" alt="Screen Shot 2023-03-19 at 12 59 23 PM" src="https://user-images.githubusercontent.com/83843298/226205952-4bcd97d5-386d-4ccd-b517-a8d4b837cd8c.png">

Maybe I'll fix this problem some day and do something useful with this code, or someone else will, or this repo will just end up lost in the code abyss.
