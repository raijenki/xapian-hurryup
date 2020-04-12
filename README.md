# The DVFS Hurryup Scheduler for Xapian (DEPRECATED)

## What is it?
DVFS Hurryup Scheduler was a proof-of-concept of a scheduling methodology. As such, It's sole purpose is to speedup cloud applications, maintaining a desired quality-of-service while also reducing energy consumption.
It currently works only with Xapian (search), which is a benchmark on the Tailbench suite[1].

## How does it works?
DVFS Hurryup aims to create a neural network model based in soujorn times and energy consumption data obtained from the combination of operating frequencies, number of active cores and queries per second. By using this model, it's possible to minimize the energy consumption for a certain imposed deadline, as the operational frequencies and number of active cores are deeply related to energy consumption.
In order to track the actual QPS, some modifications within the harness (load generator) were made in order to allow the production of real-time logs.

## Why the project was dropped?
Tailbench has some experimental flaws that required many modifications which could affect eventual reproductibility.
The first one is its inability to variate the number of server threads during its execution time. Another one was the fact that its networked version wouldn't allow a query-per-second rate of over 3-4 thousands, which wasn't enough for the experiments. The integrated version wasn't much used.

## Requisites
* Processor with dynamic voltage/frequency scaling;
* Linux 5.0 with acpi driver activated (as it allows the writing direct to the model-specific register in order to avoid overheads);
* Datasets for Tailbench software.

# Usage
The package is pretty much self-contained, albeit incomplete in functionality. 
There's only the need to configure environment variables within the scripts (e.g.: 'xapians/deps/config.sh') and to put 'xapians/deps/xapian-compiled/libxapian.so' within your '/usr/lib/' folder. For more information, refer to the 'scripts/helpers' folder.
If opting to use the modified version, the 'bin/input.test' is the QPS file. It uses the order (Time, QPS) to control the sending rate. Plus, files from the 'xapians/' directory must be compiled to 'bin/'.
Although the actual 'bin/' folder uses the instrumented version, we suggest modifying it for the networked one.

## Git structure
This repository is composed of the following folders:
* **bin**: Contains python scripts of the scheduler. Allows the formation of the database and the monitoring of the server/client application. Scheduling itself is a to-do.
* **db**: Databases generated using the 'xapian_simul' command on the 'bin/' folder.
* **docs**: Articles, presentations, thesis, etc.
* **graphs**: Illustrated graphs of some results.
* **scripts**: Contains eventual scripts to access model-specific registers, scripts to visualize graphics (in Julia) and to install eventual dependencies. 

The previous data folder was removed due to its very large size.

## Acknowledgements
* Rajiv Nishtala (Norway University of Science and Technology - Postdoctoral Researcher)
* Vinicius Petrucci (University of Pittsburgh - Visiting Scholar)
The experimental infrastructure was granted by the National Science Foundation through the Chameleon Cloud service. Part of the harness' code was derived from 'lyuhao/tailbench'.

## References
* [1] KASTURE, H. SANCHEZ, D. TailBench: A Benchmark Suite and Evaluation Methodology for Latency-Critical Applications. In: Proceedings of the 2016 IEEE International Symposium on Workload Characterization (IISWC). 2016.
