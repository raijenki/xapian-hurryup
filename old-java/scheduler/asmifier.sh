#!/bin/bash
export CLASSPATH="asm-6.0/lib/*:$CLASSPATH"
classfile=$1
shift
exec java org.objectweb.asm.util.ASMifier $classfile $@
