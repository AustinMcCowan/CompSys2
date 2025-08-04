Made by: Austin McCowan.
Class: CSC35500 Computer Systems II
Date: 02/2025

Written inside VSC, inside an Ubuntu VM using Oracle VM VirtualBox.


FUNCTION: ----------------------------------------------------------------------------------------------------------------
Takes a text file as input. Will procedurely tokenify input and attempt to perform recursive-descent parsing, creating 
"beautiful" code.

If the input file does not follow proper syntax/grammar of the parser, it will simply output that unsuccessful code was 
supplied. If a proper input file was supplied, the program will parse it together and output it to terminal

Proper grammar is as follows: 

    program     →   declaration program | compound

    declaration →   type idlist SEMICOLON

    idlist      →   ID | ID COMMA idlist

    type        →   INTEGER | FLOAT | VOID
    
    compound    →   BEGIN stmtlist END
    
    stmtlist    →   stmt | stmt SEMICOLON stmtlist
    
    stmt        →   ID | ID LPAREN exprlist RPAREN |
                    ID ASSIGNOP expr |
                    IF expr THEN compound ELSE compound |
                    WHILE LPAREN expr RPAREN compound |
                    compound
    
    exprlist    →   expr | expr COMMA exprlist
    
    expr        →   simpexpr | simpexpr RELOP simpexpr
    
    simpexpr    →   term | term ADDOP simpexpr
    
    term        →   factor | factor MULOP term
    
    factor      →   ID | ID LPAREN exprlist RPAREN |
                    ID LBRACK exprlist RBRACK |
                    NUM REAL | NUM INT |
                    LPAREN expr RPAREN

SETUP: -------------------------------------------------------------------------------------------------------------------
Program is compiled in terminal by running:
    make

Program is ran by, after doing the above, running './beautifier FILENAME' in the terminal.
FILENAME is a text file.


FILES: --------------------------------------------------------------------------------------------------------------------- 
Makefile : allows the above 'make' to function.

Token.cpp & Token.hpp : provide token functionality.

main.cpp : main file. Runs the process.

README.txt : This file.


BUGS/PROBLEMS ---------------------------------------------------------------------------------------------------------------
Be cautious of what is used as input. For best results input must follow proper grammar/syntax of the programs automata.

At one point I had bizarre segmentation errors occuring that I could not distinguish any reason why. This error some how extended 
to Project1 aswell. Occured from either bad/corrupted file or improper grammar, the error went away without me making any changes. 
All I know is that proper grammar input files still functioned fine.

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


HURDLES / JOURNEY: --------------------------------------------------------------------------------------------------------------------
At one point I considered making new .cpp and header files for the program, but decided to simply put the program inside of main.cpp this time.
One part being not wanting to tinker with makefile that much, and the other being for the sake of ease. If future programs also use this 
program, I will port functionality of this project to separate files outside of main.

Made usage of 3 global variables to significantly simplify some core elements of the programs requirements:
    
    Token nextToken : Token class object used to grab tokens. Helps with traversing the recursive-descent parsing.

    int tabcount    : Helps keep track of tabbing. No need to transfer this throughout the entire parser nonstop, nor
                      make up some algorith to predict tabbing amount based on prior tabs

    bool err        : Helps keep track of whether or not bad code (either grammar or some kind of faulty input) was recieved.
                      If any is encountered, simply set this as true.

OVERVIEW: -------------------------------------------------------------------------------------------------------------------- 
the first run of the token program creates a DFA that provides functionality akin to program specification automata (Not included in this project package/folder/zip)
The program skips comment sections and new lines alike, but does not disregard line number in the process.

Through the repeated process of calling the get method, tokens will be continuously grabbed and created.
The get process itself utilizes the previously created DFA to continuously reach states, and once a final state is reached well and proper, a token is created and finished.

In the process of recusrive-descent parsing, tokens are constantly generated and used. Inbetween these parsing decisions,
text is accumulated, including the tokens themselves, tabs, newlines, and white space. The final result, with ideal grammar input,
is beautified code output sent to terminal.

