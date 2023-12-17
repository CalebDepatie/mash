#!/usr/bin/bash

extension=".msh"
dir="./test"
daemon="./daemon/mash_d"
interp="./interpreter/build-pi/mash"

daemon_prog=$(basename $daemon)
files=$(find "$dir" -name "*$extension" -type f)

# start up the daemon
sudo $daemon > /dev/null 2>&1 &

sleep 0.2

for file in $files; do
    filename=$(basename -s $extension $file)

    test_expected=$(cat "$dir/$filename.out")

    test_actual=$($interp $file | tail -n 1)

    if [ "$test_expected" != "$test_actual" ]; then
        echo "[ FAIL ] $file"
        echo "[ FAIL ] $file expected:"
        echo $test_expected
        echo "[ FAIL ] $file actual:"
        echo $test_actual
    else
        echo "[ OK   ] $file"
    fi
done

# shut down the daemon
sudo killall -SIGINT $daemon_prog

wait