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
if [ -n $1 ];then
    kernel_path=$1
else
    kernel_path=linux-yocto-3.14/arch/x86/boot/bzImage
fi
# Original command
#qemu-system-i386 -gdb tcp::5515 -S -nographic -kernel bzImage-qemux86.bin -drive file=core-image-lsb-sdk-qemux86.ext3,if=virtio -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/vda rw console=ttyS0 debug".

# Options:
# Network pass through: Default - without any -net option - is Pass-through.
# Virtio: 
#qemu-system-i386 -gdb tcp::5515 -S -nographic -kernel $kernel_path -drive file=core-image-lsb-sdk-qemux86.ext3,if=ide -enable-kvm -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug"
qemu-system-i386 -gdb tcp::5515 -S -nographic -kernel $kernel_path -drive file=core-image-lsb-sdk-qemux86.ext3,if=virtio -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/vda rw console=ttyS0 debug"
