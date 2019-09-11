#!/bin/bash

export ES_DIR=/Users/daniel/Downloads/elasticsearch-2.4.6/bin
export INST_HOME=/Users/daniel/Downloads/elasticsearch-2.4.6/scheduler
sudo JAVA_OPTS="-javaagent:$INST_HOME/agent.jar=$(pwd)/bin/agent.txt -Djava.library.path=$INST_HOME/energymeasure" CLASSPATH=modules/lang-expression/asm-5.0.4.jar:modules/lang-expression/asm-commons-5.0.4.jar:$INST_HOME/affinity/affinity-3.1.7.jar:$INST_HOME/affinity/slf4j-api-1.7.25.jar:$INST_HOME/affinity/annotations-12.0.jar:$INST_HOME/energymeasure.jar sh bin/elasticsearch -Des.insecure.allow.root=true -Dsecurity.manager.enabled=false
echo "Running Elasticsearch"

sleep 20
python load-gen-mau-mt.py
echo "Starting load generator"


