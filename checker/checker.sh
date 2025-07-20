#!/bin/bash

test_all()
{
    cd ../src
    make clean
    ulimit -n 1024
    cd ..
    hw_checker --legacy
}

test_all
