[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/Cc2uuWhf)
[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/2Vc0gGZS)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-718a45dd9cf7e7f842a935f5ebbe5719a5e09af4491e668f4dbf3b35d5cca122.svg)](https://classroom.github.com/online_ide?assignment_repo_id=11089167&assignment_repo_type=AssignmentRepo)
# ICSH

*Tag 0.1.0* Interactive command-line interpreter
  - exho <text> : print text on the console
  - !!          : repeat the last command
  - exit <num>  : exit with the given exit code
  
*Tag 0.2.0* Script mode
  - ./icsh test.sh : return the input command content in the test.sh (in the current directory) to the console
  
*Tag 0.3.0* Running an external program in the foreground
  - run the command that already exist in SHELL

*Tag 0.4.0* Signal Handler
  - Ctrl+Z : to suspend the process in the current foreground (not your shell)
  - Ctrl+C : to kill the process in the current foreground (not your shell)
  - echo $? : to print the exit status code of the previous command. 
              You may assume that all build-in commands exits with exit code 0

*Tag 0.5.0* I/O redirection
  - reDir() : <Input/ >Output Redirection
  
*Tag 0.6.0* Background jobs and job control
  - command followed by "&" : will run as a background job
  - jobs : this command will list all the current jobs that are running/stopped
  - fg %<job_id> : Brings the job identified by <job_id> into the foreground (SIGCONT not working)
  - bg %<job_id> : Execute the suspended job identified by <job_id> in the background (SIGCONT not working)

*Tag 0.7.0* Extra features
  - Allowing users to customize their prompt at the beginning of the program (or can choose the default version)
  - help() : this command return the instructions of this IC SHELL
  - !!!! : repeat the second last command

  
  
