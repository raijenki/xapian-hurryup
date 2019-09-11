#!/bin/bash
set -e
cd src
javac org/TheAwesomeScheduler/HotFunctionTracker.java
javac -cp ".:../affinity/affinity-3.1.7.jar:../asm-6.0/lib/*:../../hotfun/src:../energymeasure" *.java && jar cfm ../agent.jar manifest.mf *.class org/TheAwesomeScheduler/*.class
