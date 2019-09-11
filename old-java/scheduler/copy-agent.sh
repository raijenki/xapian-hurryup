#!/bin/bash
top=$(dirname $0)
export CLASSPATH="$top/asm-6.0/lib/*:$top/affinity/*:$top/agent.jar:$CLASSPATH"
src=$1
dst=$2
classname=$3
classmethod=$4
shift
shift
shift
shift
exec java FileTransformer $src $dst $classname $classmethod $@
