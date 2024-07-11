#!/bin/sh
sudo cpupower frequency-set --governor performance
./build/modulo
sudo cpupower frequency-set --governor powersave