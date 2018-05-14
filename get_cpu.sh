#!/bin/bash
while [ 1 ];do
    ps aux | grep server | grep -v grep
    sleep 1
done
