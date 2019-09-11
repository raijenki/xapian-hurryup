#!/bin/bash
export CLASSPATH="asm-6.0/lib/*:$CLASSPATH:affinity/*:../hotfun/src"
details=$1
shift
exec java -javaagent:agent.jar=$details $@
