# cs444-015 Project 1:Getting Acquainted

Due: 23:59:59, 10 October 2016
===========
This is a pair exercise. Only one member of the pair should submit via TEACH. Both names need to be present in all submitted files.
For this class, we will be operating within a qemu based Yocto environment. This assignment is primarily to ensure that you can properly build the kernel and run it in qemu.
You will be required to make use of git to obtain this and all subsequent projects. Export the kernel to use from the repository at the URL 

git://git.yoctoproject.org/linux-yocto-3.14
You will need to switch to the 'v3.14.26' tag, as that is what we will be making use of.
Create a new source control repo on os-class.engr.oregonstate.edu under the /scratch/fall2016/cs444-??? directory. Please ensure you replace ??? with your assigned three digit number (include leading zeros). This will be the master copy for your code. As you work, ensure you are committing your work to this repo periodically. Choice of version control software is up to you, but you MUST maintain a central repository.
The root account of the VM is open and has no password.
You must build your kernel and run your qemu VM on os-class. If either of these fail, you will recieve no credit for the assignment. It doesn't matter if it builds and runs on your local machine, or one of the other department servers. Build on os-class, run on os-class.
In order to do the above, there is an environment configuration script to be found at /scratch/opt/environment-setup-i586-poky-linux for broken shells (bash/zsh), and /scratch/opt/environment-setup-i586-poky-linux.csh for proper shells (tcsh/csh). You must source the appropriate file prior to building the kernel or running qemu. To run qemu, make use of the command
qemu-system-i386 -gdb tcp::???? -S -nographic -kernel bzImage-qemux86.bin -drive file=core-image-lsb-sdk-qemux86.ext3,if=virtio -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/vda rw console=ttyS0 debug".
Replace the ???? in the port with your 5500 + your three digit assigned number (AS NUMBERS NOT STRINGS). Repeat: your port is 5500 + assigned number. Failure to comply with this will result in lots of frustration for you and at least one other person. Both the starting kernel and the drive file can be found in /scratch/fall2016/files/. Make copies of them in your directory, and use those. Do not use the common copy.
This will launch qemu in debug mode, with the CPU halted. In order to run the VM, connect gdb (via the yocto supplied gdb: $GBD)to a remote target at the port specified above (5500+group_number), and continue the process. This will cause the OS to boot. You really want to make sure you use the flags specified. Failure to do so will result in a painfully slow VM.
You will also need to build a new instance of the kernel, and ensure that it will boot in the VM. To do this, move to your source tree, copy /scratch/fall2016/files/config-3.14.26-yocto-qemu to $SRC_ROOT/.config (where $SRC_ROOT is the root of your linux tree), and run make -j4 all. Please do not exceed 4 threads. There are many people in this class, all of whom need access to the CPU. With 4 threads, it is a matter of minutes already, so be a little patient. Get a sandwich, get a cup of coffee, whatever. It takes about 5 minutes.
Make sure that kernel boots on the VM, and you are done with the kernel portion of this assignment.
Additionally, you will be doing a concurrency exercise. Specifically, this one.
You are also required to create a document containing the following:
A log of commands used to perform the requested actions, as well as a write-up of your concurrency solution.
An explanation of each and every flag in the listed qemu command-line
Answer the following questions in sufficient detail (for the concurrency):
What do you think the main point of this assignment is?
How did you personally approach the problem? Design decisions, algorithm, etc.
How did you ensure your solution was correct? Testing details, for instance.
What did you learn?
Version control log (formatted as a table) -- there are any number of tools for generating a TeX table from repo logs
Work log. What was done when? Be detailed.
Make sure your submission is a tarball labelled CS444_project1_#.tar.bz2 and includes the following:
A C file implementing your concurrency solution.
The write-up in LaTeX format. Failure to include LaTeX source + necessary files + makefile will result in no points awarded for the whole assignment. In other words, failure to build a PDF properly when I type "make" will result in a 0 (zero). You must go through the DVI->PS->PDF pipeline. See the course website for some LaTeX resources, including a functional makefile.
Write-up format guidelines:
Spacing: single
Font: 10 pt serif (computer modern or similar)
    Geometry: Letter paper, 0.75" margins
    Title page: required. Include name, class, title, term, abstract (at minimum)
    Abstract: required on title page
    Style: IEEETran
    Columns: 1 (use draftclsnofoot,onecolumn options to document class)
    Document type: TeX or variant + makefile
    Captions: required on all figures, must be meaningful
    Code listings: using either pygments or listings package. Use color for syntax highlighting.
    If necessary:
    Citations: in text as [x], with x being numerical order in works cited.
    Bibliography: IEEEtran style
