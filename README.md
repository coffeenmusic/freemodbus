# FreeModbus/MSP430 Fork
I ported the freemodbus/demo/MSP430 to fit on the MSP-EXP430G2 TI evaluation board with the MSP430G2553 using TI's Code Composer Studio IDE and TI compiler.
Please see the git commit history if interested in specific changes made.

I used the /demo/MSP430 as the project directory for Code Composer. Initially include files were copied in to the project directory, but later I was able to reference these files via the upper directories. This change was made in a single commit. 