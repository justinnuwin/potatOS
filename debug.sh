#!/bin/bash

qemu-system-x86_64 -s -drive format=raw,file=potatOS.img -serial stdio -display none &
gdb -x gdbinit
