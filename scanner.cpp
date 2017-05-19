#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <string.h>
using namespace std;

enum type_of_lex {
  LEX_NULL,
  LEX_AND,
  LEX_BEGIN,
  LEX_BOOL,
  LEX_DO,
  LEX_ELSE,
  LEX_END,
  LEX_IF,
  LEX_FALSE,
  LEX_INT,
  LEX_NOT,
  LEX_OR,
  LEX_PROGRAM,
  LEX_READ,
  LEX_THEN,
  LEX_TRUE,
  LEX_VAR,
  LEX_WHILE,
  LEX_WRITE,
  LEX_FIN,
  LEX_SEMICOLON,
  LEX_COMMA,
  LEX_COLON,
  LEX_ASSIGN,
  LEX_LPAREN,
  LEX_RPAREN,
  LEX_EQ,
  LEX_LSS,
  LEX_GTR,
  LEX_PLUS,
  LEX_MINUS,
  LEX_TIMES,
  LEX_SLASH,
  LEX_LEQ,
  LEX_NEQ,
  LEX_GEQ,
  LEX_NUM,
  LEX_ID,
  LEX_FUNC,
  LEX_FOR,
  LEX_IN,
  LEX_BREAK,
  LEX_CONTINUE,
  LEX_RETURN,
  LEX_STR,
  LEX_LFIG,
  LEX_RFIG,
  LEX_FUNCTYPE,
  LEX_UNDEF,
  LEX_COM,
  POLIZ_LABEL,
  POLIZ_ADDRESS,
  POLIZ_GO,
  POLIZ_FGO
};

class Lex{
    type_of_lex    t_lex;
    int             v_lex;
  public:
      Lex ( type_of_lex t = LEX_NULL, int v = 0){
          t_lex = t; v_lex = v;
      }
      type_of_lex  get_type (){
        return t_lex;
      }
      int get_value () {
        return v_lex;
      }
      friend ostream& operator << (ostream &s, Lex l) {
          s << '(' << l.t_lex << ',' << l.v_lex << ");";
          return s;
      }
};

//////////////////////////////////////////////

class ident {
  char * name;
  bool declare;
  type_of_lex type;
  bool assign;
  int value;

  public:
    ident() {
      declare = false;
      assign = false;
    }
    char* get_name() {
      return
      name;
    }
    void put_name(const char* n) {
      name = new char[strlen(n) + 1];
      strcpy(name, n);
    }
    bool get_declare() {
      return declare;
    }
    void put_declare() {
      declare = true;
    }
    type_of_lex get_type() {
      return type;
    }
    void put_type(type_of_lex t) {
      type = t;
    }
    bool get_assign() {
      return assign;
    }
    void put_assign() {
      assign = true;
    }
    int get_value() {
      return value;
    }
    void put_value(int v) {
      value = v;
    }
};


///////////////////////////////////////

class tabl_ident {
  ident* p;
  int size;
  int top;

 public:
  tabl_ident(int max_size) {
    p = new ident[size = max_size];
    top = 1;
  }
  ~tabl_ident() { delete[] p; }
  ident& operator[](int k) { return p[k]; }
  int put(const char* buf);
};

int tabl_ident::put(const char* buf) {
  for (int j = 1; j < top; ++j)
    if (!strcmp(buf, p[j].get_name()))
      return j;
  p[top].put_name(buf);
  ++top;
  return top - 1;
}

////////////////////////////////////


class Scanner {
  enum state { H, IDENT, NUMB, COM, ALE, DELIM, NEQ, STRLIT };
  static char* TW[];
  static type_of_lex words[];
  static char* TD[];
  static type_of_lex dlms[];
  state CS;
  FILE* fp;
  char c;
  char buf[80];
  int buf_top;
  void clear() {
    buf_top = 0;
    for (int j = 0; j < 80; ++j)
      buf[j] = '\0';
  }
  void add() { buf[buf_top++] = c; }
  int look(const char* buf, char** list) {
    int i = 0;
    while (list[i]) {
      if (!strcmp(buf, list[i])){
        //cout << "Found '" << buf << "' at position " << i << endl;
        return i;
      }
      ++i;
    }
    //cout << "Not found '" << buf << "' return 0\n";
    return 0;
  }
  void gc() { c = fgetc(fp); }

 public:
  Lex get_lex();
  Scanner(const char* program) {
    fp = fopen(program, "r");
    if (fp == NULL){
      perror(NULL);
      cerr << "Err open file\n";
    }
    CS = H;
    clear();
    gc();
  }
};

char* Scanner::TW[] = {
    "", // 0  позиция 0 не используется
    "function", // 1
    "var", // 2
    "if", // 3
    "else", // 4
    "while", // 5
    "for", // 6
    "do", // 7
    "in", // 8
    "break", // 9
    "continue", // 10
    "return",  // 11
    "in", // 12
    "write", //13,
    NULL
};

char* Scanner::TD[] = {
    "", // 0
    "@", // 1
    ";", // 2
    ",", // 3
    ":", // 4
    "=", // 5
    "(", // 6
    ")", // 7
    "~=", // 8
    "<", // 9
    ">", // 10
    "+", // 11
    "-", // 12
    "*", // 13
    "/", // 14
    "<=", // 15
    "!=", // 16
    ">=", // 17
    "{", // 18
    "}", // 19
    "#", // 20
    NULL
};

tabl_ident TID(100);
vector<string> TSTR;

type_of_lex Scanner::words[] = { LEX_NULL,    LEX_FUNC,   LEX_VAR, LEX_IF,
                                 LEX_ELSE,      LEX_WHILE,  LEX_FOR,   LEX_DO,
                                 LEX_IN,   LEX_BREAK,   LEX_CONTINUE,
                                 LEX_RETURN, LEX_IN, LEX_WRITE, LEX_NULL };
type_of_lex Scanner::dlms[] = {
  LEX_NULL,   LEX_FIN,    LEX_SEMICOLON, LEX_COMMA, LEX_COLON,
  LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN,    LEX_EQ,    LEX_LSS,
  LEX_GTR,    LEX_PLUS,   LEX_MINUS,     LEX_TIMES, LEX_SLASH,
  LEX_LEQ,    LEX_NEQ,    LEX_GEQ,       LEX_LFIG,  LEX_RFIG, LEX_COM,
  LEX_NULL
};

Lex Scanner::get_lex() {
  int d, j;
  CS = H;
  do {
    switch (CS) {
      case H:
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
          gc();
        else if ( c == '"'){
          //cout << "String literal";
          clear();
          gc();
          CS = STRLIT;
        }
        else if (isalpha(c)) {
          //cout << "Current symbol is '" << c << "'\n";
          clear();
          add();
          gc();
          CS = IDENT;
        } else if (isdigit(c)) {
          //cout << "Digit Current symbol is '" << c << "'\n";
          d = c - '0';
          gc();
          CS = NUMB;
        } else if (c == '#') {
         gc();
         CS = COM;
      } else if (c == ':' || c == '<' || c == '>' || c == '~') {
          //cout << "Delim symbol is '" << c << "'\n";
          clear();
          add();
          gc();
          CS = ALE;
        } else if (c == '@'){
          cerr << "@";
          return Lex(LEX_FIN);
        }else if (c == '!') {
          clear();
          add();
          gc();
          CS = NEQ;
        } else
          CS = DELIM;
        break;


      case IDENT:
        if (isalpha(c) || isdigit(c)) {
          add();
          gc();
        } else if ((j = look(buf, TW))){
          cerr << buf << " ";
          return Lex(words[j], j);
        } else {
          j = TID.put(buf);
          cerr << buf << " ";
          return Lex(LEX_ID, j);
        }
        break;


      case NUMB:
        if (isdigit(c)) {
          d = d * 10 + (c - '0');
          gc();
        } else {
          cerr << d << " ";
          return Lex(LEX_NUM, d);
        }
        break;
      case COM:
        if (c == '#') {
          gc();
          CS = H;
        } else if (c == '@' || c == '#')
          throw c;
        else
          gc();
        break;


      case ALE:
        if (c == '=') {
          add();
          gc();
          j = look(buf, TD);
          cerr << buf << " ";
          return Lex(dlms[j], j);
        } else {
          j = look(buf, TD);
          //cout << "My j is " << j << "\n";
          //cout << "My del is '" << buf << "'\n";
          cerr << buf << " ";
          return Lex(dlms[j], j);
        }
        break;


      case NEQ:
        if (c == '=') {
          add();
          gc();
          j = look(buf, TD);
          cerr << buf << " ";
          return Lex(LEX_NEQ, j);
        } else
          throw '!';
        break;


      case DELIM:
        clear();
        add();
        if ( (j = look(buf, TD)) ) {
          gc();
          cerr << buf << " ";
          return Lex(dlms[j], j);
        } else
          throw c;
        break;

      case STRLIT:
        if ( c != '"' ) {
          add();
          gc();
        } else {
          gc();
          TSTR.push_back(buf);
          j = TSTR.size() - 1;
          cerr << buf << " ";
          return Lex(LEX_STR, j);
        }
        break;


    }// end switch
  } while (true);
}

// int main(){
//   Scanner scan("test.t");
//   Lex cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#######\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#######\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#########\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#######\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#########\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//
//   cout << "#######\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#########\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#######\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#########\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#######\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#########\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#########\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#######\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//   cout << "#########\n";
//
//   cl = scan.get_lex();
//   cout << cl.get_type() << endl;
//   cout << cl.get_value() << endl;
//
//
//   return 0;
// }
