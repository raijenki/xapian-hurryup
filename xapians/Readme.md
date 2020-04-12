## Binary folder
Binary and dependencies folder for Hurryup/Xapian/Tailbench:
* **deps**: All the necessary dependencies to compile Xapian, plus harness. There are two harness: the harness-mod, which outputs realtime logs, and the harness (without modifications), which is the original one included within Tailbench.
* **xapian-hurryup**: Includes scheduling modifications on 'bin/server.cpp'.
* **xapian-mod-pure**: More suited for integrated version. It does some round-robin stuff ('main.cpp') when creating pthreads to allocate on even cores.
* **xapian-ondem-hurryup**: Don't ask me, I don't remember this anymore.
* **xapian-pure**: Tailbench's Xapian without modification.
