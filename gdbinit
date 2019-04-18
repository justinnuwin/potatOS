set arch i386:x86-64:intel
target remote localhost:1234
symbol-file kernel.bin
break kernel_main
continue
layout src
