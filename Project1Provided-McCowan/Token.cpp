/*******************************************
 * File: Token.cpp                         *
 * Author: S. Blythe                       *
 * Modified By: Austin M                   *
 * Origin Date: 12/2024                    *
 * Modified Date: 1/2025                   *
 * PURPOSE: implementation for Token       *
 *******************************************/

#include "Token.hpp"

#include <fstream>
#include <iomanip>

using namespace std;

// the promised global for string equivalents of TokenType enumeration
string TokStr[]=
{ "ERROR", "EOF_TOK", "NUM_INT", "NUM_REAL", "ADDOP", "MULOP", "ID", "RELOP", "ASSIGNOP", "LPAREN", "RPAREN",  "SEMICOLON",  "LBRACK", "RBRACK", "COMMA", "AND", "OR", "INTEGER", "FLOAT", "WHILE", "IF", "THEN", "ELSE", "VOID", "BEGIN", "END"};

// This is a "list" of the keywords. Note that they are in the same order
//   as found in the TokenType enumaration. 
static string reserved[]={"int" , "float", "while", "if", "then", "else", "void", "begin", "end" };

// Used to quickly get tokentype. ERRORS representative of a non-final state
// This is NOT exhaustive. It is common case usage.
TokenType StateToTok[] =
{ERROR, ID, NUM_INT, ERROR, NUM_REAL, ADDOP, MULOP, RELOP, RELOP, ASSIGNOP, LPAREN, RPAREN, ERROR, AND, ERROR, OR, SEMICOLON, LBRACK, RBRACK, COMMA};

/******************************************************
 *  just prints out the info describing this Token    *
 *    to specified stream                             *
 *                                                    *
 *   os  - the stream to add the Token to             *
 *                                                    *
 *   returns: the updated stream                      *
 ******************************************************/
ostream&
Token::print(ostream& os) const
{
  os
     << "{ Type:"   << left << setw(10) << TokStr[_type] 
     << " Value:"   << left << setw(10) << _value
     << " Line Number:" << _line_num
     << " }";
  return os;
}

/******************************************************
 *  Fills in information about this Token by reading  *
 *    it from specified input stream                  *
 *                                                    *
 *   is  - the stream to read the Token from          *
 *                                                    *
 *   returns: nothing                                 *
 *                                                    *
 *     **** YOU MUST CODE THIS !!!!!! ****            *
 ******************************************************/
void Token::get(istream &is)
{
  // Intialize a multi-dimensional array for automata states
  static int DFA[20][256];
  static bool firstRun = true;

  // Fill DFA
  if (firstRun) {
    // Line number is initialized at 0. It needs to be 1.
    _line_num++; 

    // Default value
    for (unsigned int row=0; row<20; row++) {
      for (unsigned int col=0; col<256; col++) { DFA[row][col] = -1; }
    }

    // ---------- State 0 transitions. ----------
    // alpha (characters)
    for (char ch='a'; ch<='z'; ch++) { DFA[0][(unsigned int) ch] = 1; }
    for (char ch='A'; ch<='Z'; ch++) { DFA[0][(unsigned int) ch] = 1; }

    // Digit
    for (char ch ='0'; ch<='9'; ch++) { DFA[0][(unsigned int) ch] = 2; }

    // ADDOP
    DFA[0][(unsigned int)'+'] = 5;
    DFA[0][(unsigned int)'-'] = 5;

    // MULOP
    DFA[0][(unsigned int)'/'] = 6;
    DFA[0][(unsigned int)'*'] = 6;

    // RELOP
    DFA[0][(unsigned int)'<'] = 7;
    DFA[0][(unsigned int)'>'] = 7;

    // ASSIGNOP 
    DFA[0][(unsigned int)'='] = 9;

    // LPAREN
    DFA[0][(unsigned int)'('] = 10;

    // RPAREN
    DFA[0][(unsigned int)')'] = 11;

    // SEMICOLON
    DFA[0][(unsigned int)';'] = 16;

    // LBRACK
    DFA[0][(unsigned int)'['] = 17;

    // RBRACK
    DFA[0][(unsigned int)']'] = 18;

    // COMMA
    DFA[0][(unsigned int)','] = 19;

    // State 12, &
    DFA[0][(unsigned int)'&'] = 12;

    // State 14, |
    DFA[0][(unsigned int)'|'] = 14;

    // ---------- State 1 Transitions. ----------
    // alpha (characters)
    for (char ch='a'; ch<='z'; ch++) { DFA[1][(unsigned int) ch] = 1; }
    for (char ch='A'; ch<='Z'; ch++) { DFA[1][(unsigned int) ch] = 1; }

    // Digit
    for (char ch ='0'; ch<='9'; ch++) { DFA[1][(unsigned int) ch] = 1; }

    // ---------- State 2 Transitions. ----------
    // Digit
    for (char ch ='0'; ch<='9'; ch++) { DFA[2][(unsigned int) ch] = 2; }

    // State 3, .
    DFA[2][(unsigned int)'.'] = 3;

    // ---------- State 3 Transitions. ----------
    // Digit
    for (char ch ='0'; ch<='9'; ch++) { DFA[3][(unsigned int) ch] = 4; }

    // ---------- State 4 Transitions. ----------
    // Digit
    for (char ch ='0'; ch<='9'; ch++) { DFA[4][(unsigned int) ch] = 4; }

    // ---------- State 5 Transitions. ----------
    /* N/A */

    // ---------- State 6 Transitions. ----------
    /* N/A */

    // ---------- State 7 Transitions. ----------
    // ASSIGNOP
    DFA[7][(unsigned int)'='] = 8;

    // ---------- State 8 Transitions. ----------
    /* N/A */

    // ---------- State 9 Transitions. ----------
    // ASSIGNOP
    DFA[9][(unsigned int)'='] = 8;

    // ---------- State 10 Transitions. ----------
    /* N/A */

    // ---------- State 11 Transitions. ----------
    /* N/A */

    // ---------- State 12 Transitions. ----------
    // &
    DFA[12][(unsigned int)'&'] = 13;

    // ---------- State 13 Transitions. ----------
    /* N/A */

    // ---------- State 14 Transitions. ----------
    // |
    DFA[14][(unsigned int)'|'] = 15;

    // ---------- State 15 Transitions. ----------
    /* N/A */

    // ---------- State 16 Transitions. ----------
    /* N/A */

    // ---------- State 17 Transitions. ----------
    /* N/A */

    // ---------- State 18 Transitions. ----------
    /* N/A */

    // ---------- State 19 Transitions. ----------
    /* N/A */

    // End.
    firstRun = false;
  }

  // Skip white space and comments. This only counts # as comments.
  char dummy;
  dummy = is.get();
  while( isspace(dummy) || dummy=='#') {

    if (dummy == '#') {           // Is the character a #? 
      while(dummy != '\n') {
        dummy = is.get();
      }
      dummy = is.get();
      _line_num++;

      is.unget();
    }
    else if (dummy == '\n') {     // Is the character a new line?
      _line_num++;
    }

    if (!is) { _type = EOF_TOK; dummy='-';} // End of file?
    else { dummy = is.get(); }              // No? Keep going.
  }
  
  // Initialize/reset variables needed to build token.
  int currState = 0;
  _value = "";

  // Check and see if we are at the end of the file.
  is.unget();
  if (!is) { type() = EOF_TOK; currState = -1; }

  // Get token.
  while(currState != -1) {
    char ch = is.get();

    // If we encounter a comment: let the next token::get handle it. The current token is also complete.
    if (ch == '#') {
      currState = -1;
    } else {
      // only grab next state if it we don't encounter a #. Token is not complete yet.
      currState = DFA[currState][(unsigned int) ch]; 
    }

    // As long as the state allows it, continue creating token.
    if (currState != -1) { 
      _value += ch; 
      _type = StateToTok[currState];
    }

    // If we know the token is an ID, we can check if it is a reserved keyword. Do not automatically assume token is finished here however.
    if ( _type == ID) {
      if (_value.compare("void") == 0) { _type = VOID; }
      if (_value.compare("int") == 0) { _type = INTEGER; }
      if (_value.compare("float") == 0) { _type = FLOAT; }
      if (_value.compare("begin") == 0) { _type = BEGIN; }
      if (_value.compare("end") == 0) { _type = END; }
      if (_value.compare("if") == 0) { _type = IF; }
      if (_value.compare("then") == 0) { _type = THEN; }
      if (_value.compare("else") == 0) { _type = ELSE; }
      if (_value.compare("while") == 0) { _type = WHILE; }
    }

  }

  is.unget();

}
