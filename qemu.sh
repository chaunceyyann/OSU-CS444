#!/bin/sh
case $SHELL in
    '/bin/tcsh') 
        echo "tcsh shell detected. Configuration done." 
        . /scratch/opt/environment-setup-i586-poky-linux.csh;;
    '/bin/csh') 
        echo "csh shell detected. Configuration done." 
        . /scratch/opt/environment-setup-i586-poky-linux.csh;;
    '/bin/bash') 
        echo "bash shell detected. Configuration done." 
        . /scratch/opt/environment-setup-i586-poky-linux;;
    '/bin/zsh') 
        echo "zsh shell detected. Configuration done." 
        . /scratch/opt/environment-setup-i586-poky-linux;;

    *) echo "No shell supported";;
esac
qemu-system-i386 -gdb tcp::5515 -S -nographic -kernel linux-yocto-3.14/arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext3,if=virtio -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/vda rw console=ttyS0 debug"
