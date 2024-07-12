#!/bin/bash

# trap ctrl-c 
trap finally INT

function finally() {
    sudo cpupower frequency-set --governor powersave 1>/dev/null
    echo "back to powersave"
}

sudo cpupower frequency-set --governor performance 1>/dev/null
./build/modulo
finally