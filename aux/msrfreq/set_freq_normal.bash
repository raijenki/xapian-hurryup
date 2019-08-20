#!/bin/bash
FREQ=${1}GHz
sudo cpufreq-set -c 0  -d ${FREQ} -u ${FREQ}
