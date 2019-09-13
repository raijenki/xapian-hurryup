# Awesome Scheduler
This is the repository for the Awesome Scheduler.

## What is it?
Awesome Scheduler is a function-level scheduler for elasticsearch tasks. It also does profiling of the Java application with use of supplementary tools.

## How it is designed to work?
In a high-level discussion, it works dynamically through the real-time profiling of applications (in the concrete case, elasticsearch). By checking the hottest functions and its critical path, the scheduler is able to determine which threads needs more priority in heterogeneous systems and allocate them accordingly.

## How do I install?
Simply clone the repository and use the start.sh script included for a quick start.

## What is the expected overhead of the scheduler?
The expected introducted overhead by running the profiling application is expected to be very high (hence why we dropped the project).

## How the file directory is structured?
* aux folder: Contains applications directly related to the Awesome Scheduler, but not programmed by the project.
* data: The collected data until now.
* docs: Contains usage documentation and related info.
* elasticsearch: Alterations to elasticsearch compilation code.
* java-agent: Instrumentation agent for Elasticsearch (Needs ASM and Thread Affinity).
* energymeasure: Library for measuring Juno energy for C and Java.
