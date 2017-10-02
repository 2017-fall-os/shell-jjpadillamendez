

# Shell - Part 2 - Project 2 - Jesus Padilla #

This directory contains:
* shell.c: implements a shell that creates a child process that runs simple commands with their arguments and other advanced features
* envrLib.c: provides required functions to manages environment variables such as: set a environment variable or expanded in an argument
* pipeLib.c: provides functions that init, connect, or close pipes for communication between processes
* vectorLib.c: implements simple functions such as free allocation of a vector or return the length of a vector
* mytoc.c: implements an algorithm that splits a string into tokens, everytime it finds a specified
  delimiter. Note: It includes two tokenizer: one that counts continous delimers as one, and other that it does not
* strlib2.c: provides our own string library. This library includes some procedures such as:
    - strlen2(*char): which counts the number of characthers in a given string
    - strcomp(*char, *char): which compares two strings and returns 1 if they are equal; otherwise returns 0
    - strconc(*char, *char): returns a new freshly-allocated string which is the concatenation of two given strings
* testShell.sh: tester provided by Dr. Freudenthal to test this shell
*testLog.txt: the latest result of running testShell.sh

To compile:
~~~
$ make all
~~~

To run it, try:
~~~
$ ./shell
~~~

To exit from the program:
~~~
$ exit
~~~

To delete generated files:
~~~
$ make clean
~~~

To test this shell
~~~
$ ./testShell.sh ./shell
~~~

TEST CASES FOR EACH REQUIRED FEATURED
  
Required Features For The Assigment
1. Simple commands (e.g. $ /bin/ls or $ ls )
  ~~~
  $ ls ..
  LICENSE  README.md  shell  shellLabDemos
  $ /bin/ls
  assert2.h  envrLib.h  mytoc.c  pipeLib.c  README.md  shell.h    strlib2.h    testShell.sh  vectorLib.h
  envrLib.c  Makefile   mytoc.h  pipeLib.h  shell.c    strlib2.c  testLog.txt  vectorLib.c
  $ echo my name is Jesus
  my name is Jesus
  $ wc assert2.h
    12  50 362 assert2.h
  $ exit
  ~~~

2. Simple pipes (e.g. $ /bin/ls | /bin/sort -r)
  ~~~
  $ ls | wc
       24      24     234
  $ ls | sort | wc
       24      24     234
  $ ls .. | sort | cat
  LICENSE
  README.md
  shell
  shellLabDemos
  $ ls .. | cat | sort | wc
        4       4      38
  $ exit
  ~~~
3. Background tasks (e.g. $ find /etc -print & )
  ~~~
  $ ls | wc & wc mytoc.c
       24      24     234           <- waits for wc mytoc.c
   164  628 5755 mytoc.c
  $ ls | wc & wc mytoc.c &          <- It does not wait for mytoc.c
   164  628 5755 mytoc.c
  $      24      24     234

  $ 
  $ ls .. | sort | wc & ls | wc & wc  <- All process at background; expect for last 'wc' which waits for input
        4       4      38
       24      24     234
  ^C
  ~~~
4. "cd" ## for "cd" you will need to lookup the library routine "chdir" in the (online) unix manual
  ~~~
  $ ls 
  assert2.h  envrLib.h  mytoc.c  pipeLib.c  README.md  shell.h    strlib2.h    testShell.sh  vectorLib.h
  envrLib.c  Makefile   mytoc.h  pipeLib.h  shell.c    strlib2.c  testLog.txt  vectorLib.c
  $ cd ..
  $ ls
  LICENSE  README.md  shell  shellLabDemo
  $ cd                      <- default is $HOME
  $ ls
  Desktop  Documents  Downloads  Music  Pictures  Public  Templates  Videos
  $ cd Documents/shell-jjpadillamendez/shell
  $ ls
  assert2.h  envrLib.h  mytoc.c  pipeLib.c  README.md  shell.h    strlib2.h    testShell.sh  vectorLib.h
  envrLib.c  Makefile   mytoc.h  pipeLib.h  shell.c    strlib2.c  testLog.txt  vectorLib.c
  $ exit
  ~~~
ADDITIONAL FEATURES WERE ADDED 

5. Redirecting input and output (e.g. $ ls > /tmp/files.txt).
  ~~~
  $ ls > test.txt
  $ cat test.txt
  LICENSE
  README.md
  shell
  shellLabDemos
  test.txt
  $ wc < test.txt
   5  5 47
  $ ls | sort | wc > test.txt
  $ cat test.txt
        5       5      47
  $ exit
  ~~~
6. Setting environment variables: "var=value".
  ~~~
  $ echo $PWD
  /home/student/Documents/shell-jjpadillamendez/shell
  $ PWD=$HOME
  $ echo $PWD
  /home/student
  $ exit
  ~~~
7. Expanding non-embedded environment variables (e.g. "echo $PATH").
  ~~~
  $ ls $HOME
  Desktop  Documents  Downloads  Music  Pictures  Public  Templates  Videos
  $ ls
  assert2.h  envrLib.h  mytoc.c  pipeLib.c  README.md  shell.h    strlib2.h    testShell.sh  vectorLib.c
  envrLib.c  Makefile   mytoc.h  pipeLib.h  shell.c    strlib2.c  testLog.txt  test.txt      vectorLib.h
  $ cd $HOME
  $ ls
  Desktop  Documents  Downloads  Music  Pictures  Public  Templates  Videos
  $ exit
  ~~~
8. Expanding embedded environment variables (e.g. "echo ${HOME}/bin"
  ~~~
  $ ls ${HOME}/Documents
  shell-jjpadillamendez  tokenizer-lab-f17-jjpadillamendez
  $ echo ${HOME}/Documents
  /home/student/Documents
  $ exit
  ~~~
9. Programs that cannot be exec'd or terminate with errors (non-zero exit codes) should be reported.
  ~~~
  If the program terminated normally but with a return value different than zero the program will notify the user. Additionally, It will notify the user if the program terminated anormally e.g., when dividing by zero
  ~~~
        
Important Note: 
 <br />- If an absolute path is not specified, this shell will try to find it by default. First, it checks at the working directory and then, it tries to find the path using the $PATH environment variable.
 <br />- The tokenizer, which is embedded in this shell, considers continuos delimiters as a single delimiter. Similarly, delimiters that are at the beginning or at the end of a given string are ignored. For example, the following two strings are considered as similar strings.
    
                    - "      ls     ..      "
                    - "ls .."
                    
 <br />- The limit of characthers in each input command is 100 characthers. Nevertheless, this length can be easily increased at the code level inside shell.c, in which there is a macro named "BUFFERLIMIT".
 <br />- Finally, the file assert2.c was borrowed from Dr. Freudenthal, which adds the feature of adding an error message to the traditional assert method.
 <br /> Note: for non-embedded and embedded variables if the user enters something that does not exit. Then, the program take it as
 empty string for example if "ls $TEMP"  and $TEMP does not exits then is equal to "ls". If the user enters a empty ${} is taken as empty string.
 <br /> Note: during testing I noticied that I made a simple mistake but because of time I was not available to correct which is that when using redirection the command cannot take parameters. The program expects something like this command > file.
    
