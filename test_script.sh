#!/bin/bash

echo "Testing MiniShell"

echo "1. Basic Commands"
ls
pwd

echo "2. Input Redirection"
echo "Hello World" > test_input.txt
cat < test_input.txt

echo "3. Output Redirection"
echo "Test Output" > test_output.txt
cat test_output.txt

echo "4. Pipelines"
ls | grep ".c" | wc -l

echo "5. Background Processes"
sleep 5 &

echo "6. Job Control"
sleep 30 &
jobs
