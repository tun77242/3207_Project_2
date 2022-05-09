USER'S MANUAL AND SHELL WORKING ABILITY

arg
  -An argument that is passed for a command to use.

batchmode
  -When the shell is run with a file containing commands, it will read the file line by line, executing the commands until the end of the file is reached.
  -My shell works by taking the commands from batch.txt file and executes the commands on to terminal,
    and once all the commands are read and executed, the shell terminates.

built-in
  -A command that is hard-coded into the shell.

cmd
  -A command.

control operator
  -A control operator allows the user to change the way in which the shell executes a command.

environment
  -A set of variables that contain information about the shell and the user's system.

environment path
  -A set of filepaths the shell uses to look for files or directories.
  -If a file or directory is not located in one of the filepaths or the cwd, it will not be opened.
  -The environment path is defaulted to /bin.
  -If the environment path is erased, only built-in commands can be run.

filename
  -A string of characters representing the name of a directory or file within the system.

filepath
  -A string of characters representing the path to a directory or file.

pipe
  -A mechanism that allows one command to redirect it's output to another command's input.

stdin
  -Standard input.
  -This is the location a process will read information from.
  -Defaults to the shell.

stdin
  -Standard output.
  -This is the location a process will read information from.
  -Defaults to the shell.

CONTROL OPERATORS

  Input Redirection

    cmd < filename
      -Changes the location of cmd's stdin to filename.
      -Anytime cmd reads from stdin, it will read from filename instead of the shell.
      -If filename is not in the cwd a filepath must be provided.

      My shell supports input redirection by reading the commands from input.txt file and executing the commands on the terminal. 

  Output Redirection

    cmd > filename
      -Changes the location of cmd's stdout to filename.
      -All of cmd's output will be sent to filename instead of the shell.
      -If filename does not exist, one is created in the cwd.
      -If filename does exist, it is truncated.
      -If filename is not in the cwd a filepath must be provided.

      My shell supports the single output redirection into a output.txt file

    cmd >> filename
      -Changes the location of cmd's stdout to filename.
      -All of cmd's output will be sent to filename instead of the shell.
      -If filename does not exist, one is created in the cwd.
      -If filename does exist, it is appended.
      -If filename is not in the cwd a filepath must be provided.

      My shell supports this double redirection and appends it into the output.txt file instead of overwriting it.

  Pipe-lining

    cmd1 | cmd2
      -Creates a pipe between cmd1 and cmd2.
      -Cmd1 will send it's output to the pipe, where it will then be read by cmd2.
      -Multiple pipes can be used to create a pipeline.
      -i.e. cmd1 | cmd2 | cmd3 | .....

      My shell supports most of the pipe lining of commands successfully.

  Background Execution

    cmd &
      -When cmd is run without this operator, the shell will pause and wait for cmd to complete before executing any more commands.
      -When this operator is used, the shell can continue executing commands as normal.
      -Must be placed after any arguments.
      
      Background execution allows the shell to run a program, and continue to execute without waiting for that program to finish.
      This is done simply by placing the '&' at the end of the line the user wishes to run in the background.  
      So 'ping google.com &' will launch the program and continue getting input from the user without waiting for it to terminate.
      My shell supports this background execution successfully.

  Parallel Execution

    cmd1 & cmd2
      -This allows both cmd1 and cmd2 to run concurrently.
      -This can be used to run as many processes concurrently as the user wants.
      -i.e. cmd1 & cmd2 & cmd3 & .....

      My shell supports parallel execution successfully for most of my commands.


BUILT-IN COMMANDS

  These commands are built into the shell and are always available for execution regardless of the path.
  They are used to interact with the shell and and navigate the system's directories.

  cd
    -Changes the cwd to filepath and prints the new cwd.
    -Only one argument can be given. 
    -Entering more than one argument results in an error.
    -If no argument is provided, the cwd is printed.

  clr
    -Clears all text from the shell. Any arguments results in an error;

  dir
    -This prints the contents of the directory specified by filepath.
    -If no filepath is given, the contents of the cwd are printed.
    -If directory does not exist or more than one argument is provided, it is an error.
    -Supports output redirection.

  environment
    -Prints the environment variables.
    -Passing an argument results in an error.
    -Supports output redirection.

  path filepath1 filepath2 .....
    -Changes the environment path to the filepaths provided. 
    -If any of the filepaths do not exist, it is an error.
    -If no filepath is provided, the environment path is erased and only built-in commands can be run until a new filepath is specified.

  echo "string"
    -Prints the string input by the user.
    -Output redirection is supported.

  help
    -Prints this readMe manual.
    -Passing an argument results in an error.
    -Output redirection is supported.

  myPause
    -Pauses the shell until enter is pressed. 
    -If any text is input before enter is pressed, the shell will remain paused.
    -Passing an argument results in an error.

  exit
    -Exits the shell. Passing an argument results in an error.

  
  Working of pipe

    -While the main process is shell, Pipeline requires 2 processes to exec on, which means 2 children.
    -Main process forks, that child1 becomes the Parent of the pipe.
    -Parent(child1) of the pipe forks. 
    -Child2 executes first command and handles input of pipe.
    -Parent(child1) executes second command and handles output of pipe.