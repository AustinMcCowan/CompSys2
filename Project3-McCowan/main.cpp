/*******************************************
 * File: main.cpp                          *
 * Original Author: S. Blythe              *
 * Modified By: Austin M                   *
 * Date: 4/2025                            *
 * PURPOSE: driver file for project 3      *
 * lexical analyzer + parser. Functionally *
 * replicates dynamic memory allocation.   *
 * This includes processes such as freeing *
 * allocated space, garbage collection,    * 
 * freelist compression, et cetera         *
 *******************************************/

#include <iostream>
#include <fstream>
#include <string> 
#include <set>
#include <unordered_map>
#include "Token.hpp"
#include "BlockList.hpp"

using namespace std;

Token nextToken;    // Greatly helps with the process of parsing
BlockList handler;  // Used throughout the entire program. Keeps track of blocknodes.

// Handles updating nextToken. Put process into a function for better readability. I hope.
void lex(istream &is) { 
  nextToken.get(is); 
}

// RECURSIVE-DESCENT PARSER =================================================================================================
// rhs element
void rhs(istream &is, string variable) {
  string asignee, num; 
  asignee = nextToken.value();
  lex(is); // Load next value.
  // If next value is LPAREN
  if (nextToken == LPAREN) {
    lex(is); // Grab num_int
    num = nextToken.value();
    lex(is); // Grab RPAREN
    // Perform allocation
    handler.allocation(variable, stoi(num));

    lex(is); // Semicolon

  // If not LPAREN
  } else {
    // perform reference
    handler.reference(variable, asignee);
  }
  
}

// stmt element
void stmt(istream &is) {
  string cmd, param;
  cmd = nextToken.value(); // Hold onto whatever the first ID is. This is either a variable or function.
  lex(is); // Load a LPAREN or ASSIGNOP

  // We are calling a function
  if (nextToken == LPAREN) {
    lex(is); // Load either RPAREN or ID
    // The function has a param
    if (nextToken == ID) {
      param = nextToken.value();
      lex(is); // Load RPAREN
      // Perform action (This is can only be free func)
      handler.free(param);
    }

    // Perform dump
    else if (cmd.compare("dump") == 0) {
      handler.dump();
    }
    // Perform compress
    else {
      handler.compress();
    }

    lex(is); // Semicolon 

  }
  // If not function, then assignop
  else {
    lex(is); // Load ID
    rhs(is, cmd);
  } 
}

// slist element
void slist(istream &is) {
  if (nextToken != EOF_TOK) {
    stmt(is);
    lex(is); // next value
    slist(is);
  }
}

// Prog element
void prog(istream &is) {
  lex(is);
  slist(is);
}

// MAIN ===========================================================================================================
int main(int argc, char *argv[])
{
  // check to make sure an input file was specified. 
  if (argc!=3)
    {
      cerr << "USAGE: " << argv[0] << " <file>" << endl;
      return -1;
    }

  // open input file
  ifstream ifile(argv[1]);

  handler.resize( stoi(argv[2]) ); // Resize avail mem to specifications
  handler.freeslot();              // Generate free space equivalent to capacity.

  // if open was not successful, let user know. 
  if (!ifile)
    {
      cerr << "ERROR: Could not open file:\"" << argv[1] << "\"" <<endl;
      return -1;
    }

  // Begin process
  prog(ifile);

  return 0;
}
