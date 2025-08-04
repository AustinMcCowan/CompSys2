/*******************************************
 * File: main.cpp                          *
 * Original Author: S. Blythe              *
 * Modified By: Austin M                   *
 * Date: 2/2025                            *
 * PURPOSE: driver file for project 2      *
 * Functionally parses together input into *
 * "beautified" code/output                *
 *******************************************/

#include <iostream>
#include <fstream>
#include <string> 
#include "Token.hpp"

using namespace std;

Token nextToken;  // Greatly helps with the process of parsing
int tabcount;     // Greatly simplifies the process of tracking and applying tabs 
bool err=false;   // Greatly simplifies the process of proclaiming unsuccessful code.

// Handles tabs. Works in conjunction with global variable tabcount for sake of ease/simplicity.
string tab() {
  string tabulation="";
  for (int i = 0; i < tabcount; i++) {
    tabulation += "\t";
  }
  return tabulation;
}

/* overloaded tab function. This is for convenience sake.
 * Allows adjusting the tab level prior to tabbing.
 * This can also be called to purely incr or decr tabcount by having
 * a standalone call to this function.
*/
string tab(signed int alter) {
  tabcount += alter;
  if (tabcount < 0) { tabcount=0; } // Just a failsafe/precaution.
  return tab();
}

// Handles updating nextToken. Put process into a function for better readability. I hope.
void lex(istream &is) { 
  nextToken.get(is); 
  if( nextToken.type()!=EOF_TOK)
    {
      // Check if we got bad input
      if (nextToken.type()==ERROR)
	    { 
        err=true;
      }
    } 
}

// RECURSIVE-DESCENT PARSER =================================================================================================
// Pre-declare some functions to solve circularity issues.
string compound(istream &is);
string expr(istream &is);
string exprlist(istream &is);

// factor element.
string factor(istream &is) {
  string text;

  // If ID
  if (nextToken == ID) 
  {
    text+=nextToken.value();
    lex(is);

    // If LPAREN
    if (nextToken == LPAREN) {
      text+= " " + nextToken.value() + " ";
      lex(is);
      text+=exprlist(is);
      if (nextToken == RPAREN) {
        text+=" " + nextToken.value();
        lex(is);
      } else { err=true; } // Expecting closing paren.
    }

    // if LBRACK
    else if (nextToken == LBRACK) { 
      text+=" " +nextToken.value() + " ";
      lex(is);
      text+=exprlist(is);

      if (nextToken == RBRACK) {
        text+=" " + nextToken.value();
        lex(is);
      } else { err=true; } // Expecting closing bracket
    }

  }

  // if a number
  else if (nextToken == NUM_REAL ||  nextToken == NUM_INT) {
    text+=nextToken.value();
    lex(is);
  }

  // if a parenthesis
  else if (nextToken == LPAREN) {
    text+=nextToken.value() + " ";
    lex(is);
    text+=expr(is);
    if (nextToken == RPAREN) {
      text+=" " + nextToken.value();
      lex(is);
    } else { err=true; } // Expecting a closing paren
  } else { err=true; } // Expecting a terminal to start with. Recieved none.

  // Return collected text.
  return text;

}

// term element
string term(istream &is) {
  string text;
  text+=factor(is);

  if (nextToken == MULOP) { 
    text+=" ";
    text+=nextToken.value();
    text+=" ";
    lex(is);
    text+=term(is);
  }

  return text;

}

// simpexpr element
string simpexpr(istream &is) {
  string text;
  text+=term(is);

  if (nextToken == ADDOP) {
    text+=" ";
    text+=nextToken.value();
    text+=" ";
    lex(is);
    text+=simpexpr(is);
  }

  return text;

}

// expr element
string expr(istream &is) {
  string text;
  text+=simpexpr(is);

  if (nextToken == RELOP) {
    text+=" ";
    text+=nextToken.value();
    text+=" ";
    lex(is);
    text+=simpexpr(is);
  }

  return text;

}

string exprlist(istream &is) {
  string text;
  text+=expr(is);

  if (nextToken == COMMA) {
    text+=nextToken.value() + " ";
    lex(is);
    text+=exprlist(is);
  }

  return text;

}

// stmt element
string stmt(istream &is) {
  string text;

  // If ID
  if (nextToken == ID) {
    text+=nextToken.value();
    lex(is);

    if (nextToken == LPAREN) {
      text+=" ";
      text+=nextToken.value();
      text+=" ";
      lex(is);
      text+=exprlist(is);

      if (nextToken == RPAREN) {
        text+=" ";
        text+=nextToken.value();
        lex(is);
      } else { err=true; }
    }

    else if (nextToken == ASSIGNOP) {
      text+=" ";
      text+=nextToken.value();
      text+=" ";
      lex(is);
      text+=expr(is);
    }
  }
  // If statement
  else if (nextToken == IF) {
    text+=nextToken.value();
    text+=" ";
    lex(is);
    text+=expr(is);
    text+=" ";

    if (nextToken == THEN) {
      text+=nextToken.value();
      text+="\n" + tab(1);
      lex(is);
      text+=compound(is);
      text+="\n" + tab(-1);

      if (nextToken == ELSE) {
        text+=nextToken.value();
        lex(is);
        text+="\n" + tab(1);
        text+=compound(is);
        tab(-1);
      } else { err=true; } // expecting an else
    } else { err=true; } // Expecting a then
  }
  // while statement
  else if (nextToken == WHILE) {
    text+=nextToken.value();
    text+=" ";
    lex(is);

    if (nextToken == LPAREN) {
      text+=nextToken.value();
      text+=" ";
      lex(is);
      text+=expr(is);
      text+=" ";

      if (nextToken == RPAREN) {
        text+=nextToken.value();
        lex(is);
        text+="\n"+tab(1);
        text+=compound(is);
        tab(-1);
      } else { err=true; } // Expecting a closing rparen.
    } else { err=true; } // Expecting an opening lparen.

  }
  // Compound
  else {
    text+=compound(is);
  }

  return text;

}

// stmtlist element.
string stmtlist(istream &is) {
  string text;
  text+=stmt(is);

  if (nextToken==SEMICOLON) {
    text+=nextToken.value();
    text+="\n"+tab();
    lex(is);
    text+=stmtlist(is);
  }

  return text;

}

// Compound element
string compound(istream &is) {
  string text;
  
  if ( nextToken == BEGIN ) {
    text+=nextToken.value();
    text+="\n"+tab(1);
    lex(is);
    text+=stmtlist(is);
    text+="\n"+tab(-1);

    if ( nextToken == END ) {
      text+=nextToken.value();
      lex(is);
    } else { err=true; } // We were expecting a mandatory END, but did not get one

  } else { err=true; } // We were expecting a mandatory BEGIN, but did not get one.

  return text;

}

// Type element.
string type(istream &is) {
  string text;

  if ( (nextToken!=INTEGER) && (nextToken!=FLOAT) && (nextToken!=VOID) ) { err=true; } // Expecting a mandatory type, but did not recieve one.
  else { 
    text+=nextToken.value();
    lex(is);
  } 
  text+=" ";
  return text;

}

// idlist element.
string idlist(istream &is) {
  string text;
  if ( nextToken == ID ) {
    text+=nextToken.value();
    lex(is);
    if (nextToken == COMMA ) {
      text+=nextToken.value();
      lex(is);
      text+=" ";
      text+=idlist(is);
    } // comma not mandatory.
    
  } else { err=true; } // Expecting a mandatory ID, did not get one.
  
  return text;

}

// declaration element.
string declaration(istream &is) {
  string text;
  text+=type(is);
  text+=idlist(is);
  if (nextToken == SEMICOLON) {
    text+=nextToken.value();
    lex(is);
  } else { err=true; } // Expecting mandatory semicolon. Failed.

  text+="\n";

  return text;

}

// Program element
string program(istream &is) {
  string text;

  // Must be assumed that a valid lex was performed prior. Otherwise, error and no reason to continue.
  if ( err ) { return ""; } 
  // Expectation that there is a 'declaration program'
  else if ( (nextToken==INTEGER) || (nextToken==FLOAT) || (nextToken==VOID) ) {  
    text+=declaration(is);
    text+=program(is);
  }
  // Expectation that there is a 'compound'
  else if ( nextToken==BEGIN ) {
    text+="\n";
    text+=compound(is);
  } 
  // If we have something other than what is above, we have bad code
  else { err=true; }

  return text;

}

// MAIN ===========================================================================================================
int main(int argc, char *argv[])
{
  // check to make sure an input file was specified. 
  if (argc!=2)
    {
      cerr << "USAGE: " << argv[0] << " <file>" << endl;
      return -1;
    }

  // open input file
  ifstream ifile(argv[1]);

  // if open was not successful, let user know. 
  if (!ifile)
    {
      cerr << "ERROR: Could not open file:\"" << argv[1] << "\"" <<endl;
      return -1;
    }
  string output;
  lex(ifile);
  output = program(ifile); 

  if ( err ) {
    cout 
        << "# UNsuccessful code ==================================" 
        << endl;
  } else {
    cout 
        << "# Successful code ===================================="
        << "\n"
        << output
        << endl;
  }

  return 0;
}
