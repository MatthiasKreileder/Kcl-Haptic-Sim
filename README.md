# Kcl-Haptic-Sim

This repo contains a ns-3 module for haptic research.

Prerequisites
=============

Please follow the installation instructions found on https://www.nsnam.org/ to obtain ns-3. The code Kcl-Haptic-Sim was linked against version 3.25 so should you by any chance encounter problems in the next steps, please check if it is a version issue.

Installation
============

- I assume you have the sources from a King's College London file server or from Github and you have put them in the folder ~/Dev/Kcl-Haptic-Sim
- I assume you have ns-3 installed inside ~/Dev/ns-3
- Link the Kcl-Haptic-Sim module against ns-3:
user@~/Dev/ns-3$ cd src
user@~/Dev/ns-3/src$ ln -s ~/Dev/Kcl-Haptic-Sim/
- Make waf include the module in the source tree:
user@~/Dev/ns-3$ ./waf configure
- Build the new sources:
./waf 

The easiest way to understand what this module can simulate is by looking at the examples inside the examples folder. Please note there are stand-alone examples which you can run simply when you have following the previous steps (marked with 'stand-alone') and others that are used for emulation (you need two PCs, chai3d and a haptic device to run this).

Examples
========

- kcl-ethernet-udp-example.cc ('stand-alone')
Demos how a haptic operator and a haptic teleoperator can be used in a local are network.
Run it with the command line switch --applyDataReduction=true to enable data reduction.
The ns-3 flow-monitor module will generate an analysis of the two data flows.

- kcl-lte-udp-congestion-example.cc ('stand-alone')
Demos how a haptic operator and a haptic teleoperator can be used in a lte network with several others nodes which generate traffic.
Run it with the command line switch --protocol=TCP to use TCP on the transport layer instead of UDP (default).
The ns-3 flow-monitor module will generate an analysis of the data flows.

- kcl-wifi-udp-example.cc 
An example of how ns-3 can emulate wifi nodes that are connected to an ethernet. If you also have downloaded the code for the chai3d simulator you can pipe your haptic device packets through ns-3.


Running the Emulation examples
==============================

user@~/Dev/ns-3$ ./waf --run "src/Kcl-Haptic-Sim/exampleName --cmdLineSwitchName=value"
