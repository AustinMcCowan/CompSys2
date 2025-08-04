Made by: Austin McCowan.
Class: CSC35500 Computer Systems II
Date: 01/2025

Written inside VSC, inside an Ubuntu VM using Oracle VM VirtualBox.


FUNCTION: ----------------------------------------------------------------------------------------------------------------
Takes a text file as input. Will tokenify the input and output it. .myl and .txt are examples of valid file types.


SETUP: -------------------------------------------------------------------------------------------------------------------
Program is compiled in terminal by running:
    make

Program is ran by, after doing the above, running './tokenify FILENAME' in the terminal.
FILENAME is a text file.


FILES: --------------------------------------------------------------------------------------------------------------------- 
Makefile : allows the above 'make' to function.

test.myl : This came default with provided resources. Therefore, it is left in. a test file.

Token.cpp & Token.hpp : provide token functionality.

main.cpp : main file. Runs the process.

README.txt : This file.


BUGS/PROBLEMS ---------------------------------------------------------------------------------------------------------------
Be cautious of what is used as input. For best results input must follow proper grammar/syntax of the programs automata.

Example: Using the output results of this very program will cause an infinite loop.
    $ ./tokenify text.myl > test.txt
    $ ./tokenify test.txt 

It is of the best interest to stick to the following characters:
    a-z
    A-Z
    0-9
    .
    *
    /
    >
    <
    =
    (
    )
    ;
    [
    ]
    ,
    &
    |


HURDLES: --------------------------------------------------------------------------------------------------------------------
Difficulty with providing accurate line numbers and proper comment skips + accounting for new lines / white space simultaneously.
Solution involved tweaking how and when characters are get'd and unget'd from istream, while also managing the tracking of state. 
Incrementing line number in the first run of token::get was also necessary, as line number is initialized at 0.


OVERVIEW: -------------------------------------------------------------------------------------------------------------------- 
Program repeatedly runs the 'get' method of the token class. 

the first run of the program creates a DFA that provides functionality akin to program specification automata (Not included in this project package/folder/zip)
The program skips comment sections and new lines alike, but does not disregard line number in the process.

Through the repeated process of calling the get method, tokens will be repeatedly formed and sent to output (which by default means
outputting the text to the console. This output can be sent into a text file if necessary. See the BUGS/PROBLEM for example if necessary.)

the get process itself utilizes the previously created DFA to continuously reach states, and once a final state is reached well and proper, a token is created and finished.