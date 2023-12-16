# MASH

This script was motivated by a desire to have a language that has high level and simple task control, but also the simplicity and OS level access of a shell.
This is not a shell, as the execution is processed on a daemon that handles the tasking.
The interactive REPL parses the input or file and sends an execution stream to the local daemon for processing, which evaluates and returns the result of each line.

This simplifies the most important feature for me, system global signalling for very simple user scheduling of a task.
It also enables the simplification of some other interesting features, like sharing signals between networked machines.

It is important to note that there is no safety involved in this system, starvation, dead-locks, live-locks, ect are all to be managed by the programmer.
For a large distributed system, this is certainly a drawback, and perhaps more safety indications could be built into either the daemon or the interactive portion in the future.
