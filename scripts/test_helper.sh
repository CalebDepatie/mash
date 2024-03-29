#!/usr/bin/bash

# Run and evaluate integration tests in ./test
# provides little debug information

source ./scripts/script_core.sh

# start up the daemon
sudo $daemon > /dev/null 2>&1 &

sleep 0.2

for file in $files; do
    filename=$(basename -s $extension $file)

    test_expected=$(cat "$dir/$filename.out")

    start_time=$(date +%s%N)  

    test_actual=$($interp $file | tail -n 1)

    end_time=$(date +%s%N)  
    execution_time=$(( (end_time - start_time) / 1000000 )) 


    if [ "$test_expected" != "$test_actual" ]; then
        echo "[ FAIL ] $file took $execution_time ms"
        echo "         $file expected: $test_expected"
        echo "         $file actual: $test_actual"
    else
        echo "[ OK   ] $file took $execution_time ms"
    fi
done

# shut down the daemon
sudo killall -SIGINT $daemon_prog

wait