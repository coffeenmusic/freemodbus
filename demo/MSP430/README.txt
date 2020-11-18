
                      FREEMODBUS 1.1 MSP430 PORT
                      ==========================

REQUIREMENTS
============

This demo application provides a port for  the	MSP430	series	of  processors
from  Text  Instruments.   The	port  was  done  using	the  MSP-EXP430G2  from
TI[1] which features a MSP430G2553 MCU.
It requires a wired serial port to  a  host  processor	and  a	Modbus	Master
Software on the PC side  to  be  useful.   Demo  versions  of  Modbus  Masters
can be found in [2] and [3].

INSTALLATION
============

SOURCE
======

TESTING
=======

Start the Modbus Sample Application and test if the input  registers  starting
at protocol address 1000 can be read. There are four registers value available
and the output should look like:

Polling slave (Ctrl-C to stop) ...
[1000]: 6474
[1001]: 0
[1002]: 0
[1003]: 0
Polling slave (Ctrl-C to stop) ...
[1000]: -8831
[1001]: 0
[1002]: 0
[1003]: 0
Polling slave (Ctrl-C to stop) ...

The simple testing utility used in the 'demo_rtu.bat' script can be found
at [4].

PROBLEMS
========

[1] TI MSP-EXP430G2 kit: https://www.ti.com/tool/MSP-EXP430G2
[2] WinTech ModScan32: http://www.win-tech.com/html/modscan32.htm
[3] Modus Poll: http://www.modbustools.com/modbus_poll.asp
[4] FieldTalk Modpoll: http://www.focus-sw.com/fieldtalk/modpoll.html

Version: $Id: README.txt,v 1.1 2020/11/18 03:00:33 wolti Exp $
