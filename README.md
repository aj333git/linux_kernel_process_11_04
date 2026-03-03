# Linux Kernel Process Inspect Module

A Linux kernel module that exposes process information via /proc.

## Build

make

## Sign Module (Secure Boot)

sudo /usr/src/linux-headers-$(uname -r)/scripts/sign-file sha256 \
~/kernel_keys/MOK.key \
~/kernel_keys/MOK.crt \
proc_inspect.ko

## Load Module

sudo insmod proc_inspect.ko

## Show All Processes

sudo cat /proc/proc_inspect

## Show Specific PID

echo 1 | sudo tee /proc/proc_inspect
sudo cat /proc/proc_inspect

## Switch Back to All

echo all | sudo tee /proc/proc_inspect

## Remove Module

sudo rmmod proc_inspect

## Debug

lsmod | grep proc_inspect
dmesg | tail

