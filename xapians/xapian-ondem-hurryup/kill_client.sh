#!/bin/bash

kill -9 $(cat client.pid)
rm client.pid
