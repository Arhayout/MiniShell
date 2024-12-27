# Testing Commands for MiniShell
---

## 1. Basic Commands
```bash
ls
pwd
whoami
date
```

---

## 2. Input and Output Redirection
### Input Redirection:
```bash
cat < input.txt
```

### Output Redirection:
```bash
echo "Hello, MiniShell!" > output.txt
cat output.txt
```

### Combined Input and Output Redirection:
```bash
grep "pattern" < input.txt > output.txt
```

---

## 3. Pipelines
Run commands connected by pipelines to ensure proper execution:
```bash
ls -l | grep ".c" | wc -l
cat input.txt | sort | uniq
```

---

## 4. Background Processes
Run commands in the background and manage them:
```bash
sleep 10 &
```

### List Jobs:
```bash
jobs
```

---

## 5. Job Control
### Bring a Background Job to Foreground:
```bash
fg <job_id>
```

### Stop a Job:
Run a command, then press `Ctrl+Z` to stop it. Verify with:
```bash
jobs
```

### Resume a Job in the Background:
```bash
bg <job_id>
```

---

## 6. Signal Handling
### Interrupt a Command (`Ctrl+C`):
Run a long-running command, e.g.,
```bash
sleep 30
```
Press `Ctrl+C` to terminate it.

### Stop a Command (`Ctrl+Z`):
Run a command, e.g.,
```bash
sleep 30
```
Press `Ctrl+Z` to stop and move it to the background.

---

## 7. Edge Cases
### Empty Command:
Press Enter without typing anything.

### Invalid Command:
```bash
nonexistent_command
```

### Pipe with No Following Command:
```bash
ls |
```

---

## 8. Custom Commands
### Change Directory:
```bash
cd /
pwd
cd ..
pwd
```

### Exit MiniShell:
```bash
exit
```

### List Jobs:
```bash
jobs
```

---

## 9. Redirection Errors
Test invalid redirections to ensure error handling:
```bash
cat < nonexistent_file
echo "Test" > /
```

---

## 10. Automated Testing

### Test Script:
```bash
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
```

### Run the Test Script:
```bash
./test_script.sh
