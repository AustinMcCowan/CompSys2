COMPSYS2 PROJECT 3 - Garbage Collection

Made by: Austin McCowan.
Class: CSC35500 Computer Systems II
Date: 04/2025

Written inside VSC, inside an Ubuntu VM using Oracle VM VirtualBox.


FUNCTION: ----------------------------------------------------------------------------------------------------------------
An interpreter that supports allocating dynamic memory to variables, copying variables (references), free allocated space,
garbage collection, and freelist compression.

Takes a text file and memory size as input. Will procedurely tokenify input and attempt to perform recursive-descent parsing. 
Through this parsing, the program will attempt to perform the prior listed functionalities.

Note: Free List is ordered by spot in "memory".

Proper BNF grammar is as follows: 

    prog        →   slist

    slist       →   stmt SEMICOLON slist | e

    stmt        →   ID LPAREN ID RPAREN | ID LPAREN RPAREN | ID ASSIGNOP rhs

    rhs         →   ID LPAREN NUM_INT RPAREN | ID

Supported functions:
    free(variableName) : Will dereference the specified variable, decrementing the reference count of whatever it referenced. Possibly,
                         this will result in returning the referenced storage to free list.

    dump() : Will print all allocated variables (alongside their name, location in memory, size of allocation, and reference count) 
             and blocks in free list (These blocks will also share their location in memory, size of allocation, and reference count)
    
    compress() : Join adjacent blocks in the free list into an appropriately located and sized larger block.
    
    variableName = alloc(integerAmount) : Uses First Fit in the free list to find space to allocate the requested integer amount of space,
                                          will then allocate that block to the variable name specified.
    
    variableName = otherVariableName : Will make variableName reference whatever is referenced by otherVariableName. This will update blocks
                                       accordingly (and possibly send them to free list as a result).

SETUP: -------------------------------------------------------------------------------------------------------------------
Program is compiled in terminal by running:
    make

This will create an executable called garbo.
Program is ran by, after doing the above, running './garbo FILENAME MEMORY_SIZE' in the terminal.
FILENAME is a text file, MEMORY_SIZE is an integer amount. Output may alter based on specified memory size.


FILES: --------------------------------------------------------------------------------------------------------------------- 
Makefile : allows the above 'make' to function.

Token.cpp & Token.hpp : provide token functionality.

main.cpp : main file. Runs the process.

BlockNode.cpp & BlockNode.hpp : provide the functionality of a 'block' object.

BlockList.cpp & BlockList.hpp : provide program functionality utilizing the above 'block' objects.

README.txt : This file.

test.txt, test2.txt, test3.txt : Example test files for use.

BUGS/PROBLEMS ---------------------------------------------------------------------------------------------------------------
Be cautious of what is used as input. For best results input must follow proper grammar/syntax of the programs automata.

This program also generates id's (used as keys for unordered_map) for blocks by way of using an integer within BlockList that increments 
once everytime a new block is created. Which means the limit for block generation is UINT_MAX. This should in theory not cause any issues 
unless, for some reason, you decide to create billions of blocks. (Which also means you created a text file for input that is roughly many 
Gigabytes in size at minimum).

<<<<<<<<<< TEXT FILES USED AS INPUT MUST (!!!) END WITH WHITE SPACE (Space or newline). THE PROGRAM WILL STILL RUN FINE, >>>>>>>
<<<<<<<<<< BUT WILL END WITH A SEGMENTATION FAULT OTHERWISE. The project's provided test file follows this standard,     >>>>>>>
<<<<<<<<<< including a newline to end the file.                                                                          >>>>>>>

HURDLES / JOURNEY: --------------------------------------------------------------------------------------------------------------------
At first, attempted to create a tree structure for freeList where each free block would link to both the next free block in memory as
well as a linked list to the allocated blocks between it and the next free block. After awhile I realized it was simpler to just have the entirety of this
be one big doubly linked list, sacrificing the pointer towards next free blocks. However, this does mean some processes now move at
runtime O(N)~ instead of run time of O(1)~ compared relatively given a high amount of allocated blocks. If there is a high amount of free blocks instead
of allocated blocks, however, the performance difference decreases (since iteration through free blocks is identical for both).

That being said, output wise, the program still operates the same as what is intended.

I have also done a shoddy job of adjusting the makefile. Apologies. I am not all that familiar with setting up makefiles. It still should
do what is needed (Based off of it working for my computer and environment). 

OVERVIEW: -------------------------------------------------------------------------------------------------------------------- 
Variable references are stored within unordered_maps (stored as <VariableName, BlockNode*>). In addition, blocks are also stored within a unordered_map
for reference purposes as <id, BlockNode>. The placement of values in these are unimportant, as the ordering of them are handled by the double linked list
functionality.

If a free block runs out of space, it is deleted.

Changing references can cause an allocated block to become free. In addition, when switching references for a variable, it will always dereference its
prior referenced memory before allocating/referencing its new one.

This is also true for making a pre-existing variable reference a newly allocated variable. In this process, the currently referenced variable will be
dereferenced first, THEN the program will search the freelist for a spot that allows the allocation to occur. Therefore, the process of calling
variable = alloc(INT) will always works if its new allocation is smaller than its previous reference (and its previous reference became free as a result).

Note: If an allocation cannot occur, it WILL NOT OCCUR. the program will not fail as a result.
In addition, if you try to reference a variable that doesn't exist (For example, the previously mentioned allocation did not occur due to a lack of space),
the program will also not fail the program. 

As a result, the output of the program based on your input will vary based on memory size specified. 

Another note: This program will NOT create "ghost" variables, nor let variables exist without referencing anything. 
For example the following input file test case:

    d=a; dump();
    d=b; dump();
    a=alloc(3); dump();
    a=d; dump(); 
    a=c; dump(); 

The first two references will NOT create variables. d, b, and a WILL NOT be created. As a result, the program will output the following (with 512 size):

    Variables:
    Free List:
    0(512) [0]
    ============================================================================
    Variables:
    Free List:
    0(512) [0]
    ============================================================================
    Variables:
    a:0(3) [1]
    Free List:
    3(509) [0]
    ============================================================================
    Variables:
    Free List:
    0(3) [0], 3(509) [0]
    ============================================================================
    Variables:
    Free List:
    0(3) [0], 3(509) [0]
    ============================================================================

Notice 'd' does not exist, ever. Also notice 'a' disappears when we try to reference 'd'. This program, like previously mentioned, WILL remove 
previously existing variables if you try to make them reference nothing.