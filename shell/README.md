

# Shell - Part 1 - Project 2 - Jesus Padilla #

This directory contains:
* shell.c: implements a shell that creates a child process that runs simple commands with their arguments
* mytoc.c: implements an algorithm that splits a string into tokens, everytime it finds a specified
  delimiter.
* strlib2.c: provides our own string library. This library includes some procedures such as:
    - strlen2(*char): which counts the number of characthers in a given string
    - strcomp(*char, *char): which compares two strings and returns 1 if they are equal; otherwise returns 0
    - rmCharIn(*char,char): which removes the given char at the first index that appears in the string
    - strconc(*char, *char): concatenates two strings
    - copystr(*char): returns a new freshly-allocated copied string

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

The following test cases were run to debug this shell

      Test Cases:
            $ ls
            $ ls ..
            $ mkdir tempDir
            $ cat shell.h
            $ make clean
            $ fakecommand
            
            
The following screenshot displays the output of the program after running these test cases.

![Alt text](https://github.com/2017-fall-os/shell-jjpadillamendez/blob/master/shell/testcases.jpg?raw=true)

How to used this program:
     <br />1. After running the program named 'shell', the user can type any simple command and the shell
            will create a child process; which runs the command using execve.
         <br />- Note: simple pipes, background tasks, and cd are still not implemented
     <br />2. At any moment, the user can type 'exit' to stop this shell
        
Important Note: 
 <br />If an absolute path is not specified, this shell will try to find it by default. First, it checks at the working directory and then, it tries to find the path using the $PATH environment variable.
 <br />The tokenizer, which is embedded in this shell, considers continuos delimiters as a single delimiter. Similarly, delimiters that are at the beginning or at the end of a given string are ignored. For example, the following two strings are considered as similar strings.
    
                    - "      ls     ..      "
                    - "ls .."
                    
 <br />The limit of characthers in each input command is 100 characthers. Nevertheless, this length can be easily increased at the code level inside shell.c, in which there is a macro named "BUFFERLIMIT".
 <br />Finally, the file assert2.c was borrowed from Dr. Freudenthal, which adds the feature of adding an error message to the traditional assert method.
    
