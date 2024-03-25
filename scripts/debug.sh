#!/usr/bin/bash

# Run the tests in ./test
# provides all information nessecary for debugging, and does not evaluate a test
# Run a test file by passing the filename as an argument

source ./scripts/script_core.sh

# start up the daemon (Leaving on logging)
sudo $daemon &

sleep 0.2

# Run the file and print all information

echo "*** Debugging $1 ***"
echo ""

test_actual=$($interp "$dir/$1$extension")
echo -n $test_actual
echo ""

# shut down the daemon
sudo killall -SIGINT $daemon_prog

wait