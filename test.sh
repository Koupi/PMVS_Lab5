#!/bin/bash
insmod calculator.ko
`dmesg | grep -m 1 'mknod /dev/first' | awk -F "'" {'print $2'}`
`dmesg | grep -m 1 'mknod /dev/second' | awk -F "'" {'print $2'}`
`dmesg | grep -m 1 'mknod /dev/operand' | awk -F "'" {'print $2'}`

function check {
    echo $1 > /dev/first
    echo $2 > /dev/operand
    echo $3 > /dev/second
    if [ "$4" == "`cat /proc/result`" ]; then
        echo "True!"
    else
        echo "False!"
    fi
}

check 3 + 2 "  5"
check 20 + 2 " 22"
check 77 + 33 "110"

check 20 - 18 "  2"
check 44 - 33 " 11"
check 2 - 1 "  1"

check 55 / 11 "  5"
check 33 / 11 "  3"
check 21 / 7 "  3"

check 20 m 3 " 60"
check 31 m 3 " 93"
check 44 m 1 " 44"

rmmod calculator
rm /dev/operand
rm /dev/first
rm /dev/second

