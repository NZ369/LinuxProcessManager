# LinuxProcessManager
A process manager called PMan (custom linux shell) that supports running and managing background processes running on the operating system.

********************************************************************************************************************************************
********************************************************************************************************************************************

Procedures for Running Process Manager PMan

[Note: PMan code runs on linux environments

1. To compile code: On terminal execute `make` command.
2. Ensure executable files (*.o) of testing programs are present within the same directory (PMan folder).
3. Ensure these executable files (*.o) of testing programs were compiled in the linux system.
3. If no executable files (*.o) of testing programs are available, include the (*.c) files of the programs
   in question and add the following compiling code to the Makefile. 

            all: pman inf <filename>

            <filename>: <filename>.c
                gcc -o <filename> <filename>.c

4. To run PMan executable: On terminal execute `./PMan` command.

The PMan: > is comprised of 6 commands:

1. bg <cmd>         [ In which process <cmd> starts running in the background ]
2. bglist           [ In which all processes commenced by the system will be listed ]
3. bgkill <pid>     [ In which the process with process id <pid> will be terminated ]
4. bgstop <pid>     [ In which the process with process id <pid> will be stopped ]
5. bgstart <pid>    [ In which the process with process id <pid> will be restarted ]
6. pstat <pid>      [ In which the process with process id <pid> will have /proc information displayed ]

********************************************************************************************************************************************
********************************************************************************************************************************************
