#include "scanner.cpp"

template <class T, int max_size>
class Stack {
  T s[max_size];
  int top;

 public:
  Stack() { top = 0; }
  void reset() { top = 0; }
  void push(T i);
  T pop();
  bool is_empty() { return top == 0; }
  bool is_full() { return top == max_size; }
};

template <class T, int max_size>
void Stack<T, max_size>::push(T i) {
  if (!is_full()) {
    s[top] = i;
    ++top;
  } else
    throw "Stack_is_full";
}

template <class T, int max_size>
T Stack<T, max_size>::pop() {
  if (!is_empty()) {
    --top;
    return s[top];
  } else
    throw "Stack_is_empty";
}

/////////////////////////////////////////////////////////

class Poliz {
  Lex* p;
  int size;
  int free;

 public:
  Poliz(int max_size) {
    p = new Lex[size = max_size];
    free = 0;
  };
  ~Poliz() { delete[] p; };
  void put_lex(Lex l) {
    p[free] = l;
    ++free;
  };
  void put_lex(Lex l, int place) { p[place] = l; };
  void blank() { ++free; };
  int get_free() { return free; };
  Lex& operator[](int index) {
    if (index > size)
      throw "POLIZ:out of array";
    else if (index > free)
      throw "POLIZ:indefinite element of array";
    else
      return p[index];
  };
  void print() {
    for (int i = 0; i < free; ++i) cerr << p[i];
  };
};

/////////////////////////////////////////////////////////


class Parser {
  Lex curr_lex;
  type_of_lex c_type;
  int c_val;
  Scanner scan;
  Stack <int, 100> st_int;
  Stack <type_of_lex, 100> st_lex;

  void P();
  void S();
  void FD1();
  void OP();
  void VD1();
  void BLOCK1();
  void COND1();
  void LOOP1();
  void LOOP2();
  void LOOP3();
  void JOP1();
  void VE();
  void BLOCK();

  // Expression
  void E();
  void E1();
  void T();
  void F();

  void dec(type_of_lex type); // семантичиеские действия
  void check_id();
  void check_op();
  void check_not();
  void eq_type();
  void eq_bool();
  void check_id_in_read();
  void declare_id(type_of_lex type);

  void gl() {
    curr_lex = scan.get_lex();
    c_type = curr_lex.get_type();
    c_val = curr_lex.get_value();
  }

public:
  Poliz prog;
  Parser(const char* program): scan(program), prog(1000) {}
  void analyze();
};

//////////////////////////////////////////////////////////////


void Parser::declare_id(type_of_lex type) {
    if (TID[c_val].get_declare())
      throw "twice";
    else {
      TID[c_val].put_declare();
      TID[c_val].put_type(type);
    }
}


void Parser::dec(type_of_lex type) {
  int i;
  while (!st_int.is_empty()) {
    i = st_int.pop();
    if (TID[i].get_declare())
      throw "twice";
    else {
      TID[i].put_declare();
      TID[i].put_type(type);
    }
  }
}

void Parser::check_id(){
  if (TID[c_val].get_declare())
    st_lex.push(TID[c_val].get_type());
  else
    throw "not declared";
}

void Parser::check_op() {
  type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;
  t2 = st_lex.pop();
  op = st_lex.pop();
  t1 = st_lex.pop(); //cerr << t1 << " " << t2;
  if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH)
    r = LEX_INT;
  if (op == LEX_OR || op == LEX_AND) t = LEX_BOOL;
  if (t1 == t2 && t1 == t)
    st_lex.push(r);
  else
    throw "wrong types are in operation";
  prog.put_lex( Lex(op) );
}

void Parser::check_not() {
  if (st_lex.pop() != LEX_BOOL)
    throw "wrong type is in not";
  else {
    st_lex.push(LEX_BOOL);
  }
  prog.put_lex( Lex(LEX_NOT) );
}

void Parser::eq_type() {
  if (st_lex.pop() != st_lex.pop()) throw "wrong types are in =";
}

void Parser::eq_bool() {
  if (st_lex.pop() != LEX_BOOL) throw "expression is not boolean";
}

void Parser::check_id_in_read() {
  if (!TID[c_val].get_declare()) throw "not declared";
}

void Parser::analyze() {
  gl();
  P();
  //prog.print();
  cerr << endl << "Sintax analisys is completed" << endl;
}

void Parser::P(){
  while (c_type != LEX_FIN) {
    S();
  }
  gl();
}

void Parser::S(){
  if ( c_type == LEX_FUNC ){
    gl();

    FD1();
  }
  else OP();
}


void Parser::FD1(){
  if ( c_type != LEX_ID ) throw curr_lex;
  declare_id(LEX_FUNCTYPE);
  gl();

  if ( c_type != LEX_LPAREN ) throw curr_lex;
  gl();


  if ( c_type == LEX_ID ){
    declare_id(LEX_UNDEF);
    gl();

    while ( c_type == LEX_COMMA ) {
      gl();

      if ( c_type != LEX_ID ) throw curr_lex;
      declare_id(LEX_UNDEF);
      gl();
    }
  }
  if ( c_type != LEX_RPAREN ) throw curr_lex;
  gl();

  BLOCK();
}

void Parser::OP(){
  if ( c_type == LEX_VAR ){
    gl();

    VD1();
  } else if ( c_type == LEX_SEMICOLON ){
    gl();

    // do nothing
  } else if ( c_type == LEX_LFIG ){
    gl();

    BLOCK1();
  } else if ( c_type == LEX_IF ){
    gl();

    COND1();
  } else if ( c_type == LEX_WHILE ){
    gl();

    LOOP1();
  } else if ( c_type == LEX_DO ){
    gl();

    LOOP2();
  } else if ( c_type == LEX_FOR ){
    gl();

    if ( c_type != LEX_LPAREN ) throw curr_lex;
    gl();

    LOOP3();

  } else if ( c_type == LEX_BREAK || c_type == LEX_CONTINUE) {
    gl();
    // do nothing
  } else if ( c_type == LEX_RETURN ){
    gl();

    JOP1();
  } else if ( c_type == LEX_WRITE ) {
    gl();
    //cerr << "IM IN WRITE\n";

    if ( c_type != LEX_LPAREN ) throw curr_lex;
    gl();

    E();

    if ( c_type != LEX_RPAREN ) throw curr_lex;
    gl();
    prog.put_lex( Lex(LEX_WRITE) );

  } else {
    VE();
    if ( c_type != LEX_SEMICOLON ) throw curr_lex;
    gl();
  }
}

void Parser::VD1(){
  int curv;
  if ( c_type != LEX_ID ) throw curr_lex;
  declare_id(LEX_UNDEF);
  curv = c_val;
  gl();

  if ( c_type == LEX_ASSIGN ){
    gl();

    prog.put_lex( Lex(POLIZ_ADDRESS, curv) );

    E();
    prog.put_lex( Lex(LEX_ASSIGN) );
    TID[curv].put_type(st_lex.pop());
  }
  while ( c_type == LEX_COMMA ) {
    gl();

    if ( c_type != LEX_ID ) throw curr_lex;
    declare_id(LEX_UNDEF);
    curv = c_val;
    gl();

    if ( c_type == LEX_ASSIGN ){
      gl();

      prog.put_lex( Lex(POLIZ_ADDRESS, curv) );

      E();
      prog.put_lex( Lex(LEX_ASSIGN) );
      TID[curv].put_type(st_lex.pop());
    }
  }
  if ( c_type != LEX_SEMICOLON ) throw curr_lex;
  gl();
}

void Parser::BLOCK1(){
  OP();

  while (c_type != LEX_RFIG) {
    OP();
  }
  gl();
}

void Parser::COND1(){
  if ( c_type != LEX_LPAREN ) throw curr_lex;
  gl();

  E();
  eq_bool();
  int pl2 = prog.get_free();
  prog.blank();
  prog.put_lex( Lex(POLIZ_FGO) );


  if ( c_type != LEX_RPAREN ) throw curr_lex;
  gl();

  OP();
  if ( c_type == LEX_ELSE ){
    int pl3 = prog.get_free();
    prog.blank();
    prog.put_lex( Lex(POLIZ_GO) );
    prog.put_lex( Lex(POLIZ_LABEL, prog.get_free()), pl2 );

    gl();

    OP();
    prog.put_lex( Lex(POLIZ_LABEL, prog.get_free()), pl3 );
  } else {
    prog.put_lex( Lex(POLIZ_LABEL, prog.get_free()), pl2 );
  }
}

void Parser::LOOP1(){
  if ( c_type != LEX_LPAREN ) throw curr_lex;
  gl();

  int pl0 = prog.get_free();
  E();
  eq_bool();
  int pl1 = prog.get_free();
  prog.blank();
  prog.put_lex( Lex(POLIZ_FGO) );

  if (c_type != LEX_RPAREN ) throw curr_lex;
  gl();

  OP();
  prog.put_lex( Lex(POLIZ_LABEL, pl0) );
  prog.put_lex( Lex(POLIZ_GO) );
  prog.put_lex( Lex(POLIZ_LABEL, prog.get_free()), pl1 );
}

void Parser::LOOP2(){
  int pl0 = prog.get_free();
  OP();

  if ( c_type != LEX_WHILE ) throw curr_lex;
  gl();

  if ( c_type != LEX_LPAREN ) throw curr_lex;
  gl();

  E();
  eq_bool();
  int pl1 = prog.get_free();
  prog.blank();
  prog.put_lex( Lex(POLIZ_FGO) );
  prog.put_lex( Lex(POLIZ_LABEL, pl0) );
  prog.put_lex( Lex(POLIZ_GO) );
  prog.put_lex( Lex(POLIZ_LABEL, prog.get_free()), pl1 );

  if ( c_type != LEX_RPAREN ) throw curr_lex;
  gl();

  if ( c_type != LEX_SEMICOLON) throw curr_lex;
  gl();
}

void Parser::LOOP3(){
  if ( c_type == LEX_VAR ){
    gl();

    if ( c_type != LEX_ID ) throw curr_lex;
    declare_id(LEX_UNDEF);
    gl();

    if ( c_type != LEX_IN ) throw curr_lex;
    gl();

    E();

    if ( c_type != LEX_RPAREN ) throw curr_lex;
    gl();

    OP();
  } else {
    if ( c_type == LEX_SEMICOLON ){
      gl();
    } else {
      VE();

      if ( c_type != LEX_SEMICOLON ) throw curr_lex;
      gl();
    }

    int pl0, pl1, pl2;// pl3, pl4
    int pl5, pl6;

    if ( c_type == LEX_SEMICOLON ){
      gl();
    } else {
      pl0 = prog.get_free();
      E();
      eq_bool();
      pl1 = prog.get_free();
      prog.blank();
      prog.put_lex( Lex(POLIZ_FGO) );

      pl2 = prog.get_free();
      prog.blank();
      prog.put_lex( Lex(POLIZ_GO) );

      if ( c_type != LEX_SEMICOLON ) throw curr_lex;
      gl();
    }



    if ( c_type == LEX_RPAREN ){
      gl();
    } else {
      pl5 = prog.get_free();
      VE();

      if ( c_type != LEX_RPAREN ) throw curr_lex;
      gl();
    }

    prog.put_lex( Lex(POLIZ_LABEL, pl0));
    prog.put_lex( Lex(POLIZ_GO) );

    pl6 = prog.get_free();
    OP();
    prog.put_lex( Lex(POLIZ_LABEL, pl5) );
    prog.put_lex( Lex(POLIZ_GO) );

    // initialize blanks
    prog.put_lex( Lex(POLIZ_LABEL, prog.get_free()) , pl1 );
    prog.put_lex( Lex(POLIZ_LABEL, pl6), pl2 );
  }
}

void Parser::JOP1(){
  if ( c_type == LEX_SEMICOLON ){
    gl();
  } else {
    E();

    if ( c_type != LEX_SEMICOLON ) throw curr_lex;
    gl();
  }
}

void Parser::VE(){
  if ( c_type == LEX_ID ){
    check_id();
    st_lex.pop(); // var type isn't needed
    prog.put_lex( Lex( POLIZ_ADDRESS, c_val ) );
    int curv = c_val;
    gl();

    if ( c_type != LEX_ASSIGN ) throw curr_lex;
    gl();
    E();
    TID[curv].put_type(st_lex.pop());// get an expr type from stack
    prog.put_lex( Lex( LEX_ASSIGN ) );
  } else E();
}

void Parser::BLOCK(){
  if ( c_type != LEX_LFIG ) throw curr_lex;
  gl();

  BLOCK1();
}

void Parser::E() {
  E1();
  if (c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_GTR ||
      c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ) {
    st_lex.push(c_type);
    gl();
    E1();
    check_op();
  }
}

void Parser::E1() {
  T();
  while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR) {
    st_lex.push(c_type);
    gl();
    T();
    check_op();
  }
}

void Parser::T() {
  F();
  while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_AND) {
    st_lex.push(c_type);
    gl();
    F();
    check_op();
  }
}

void Parser::F() {

  if (c_type == LEX_ID) {
    check_id();
    prog.put_lex(Lex(LEX_ID, c_val));
    gl();
  } else if (c_type == LEX_NUM) {
    st_lex.push(LEX_INT);
    prog.put_lex(curr_lex);
    gl();
  } else if (c_type == LEX_TRUE) {
    st_lex.push(LEX_BOOL);
    prog.put_lex(Lex(LEX_TRUE, 1));
    gl();
  } else if (c_type == LEX_FALSE) {
    st_lex.push(LEX_BOOL);
    prog.put_lex(Lex(LEX_FALSE, 0));
    gl();
  } else if (c_type == LEX_NOT) {
    gl();
    F();
    check_not();
  } else if (c_type == LEX_LPAREN) {
    gl();
    E();
    if (c_type == LEX_RPAREN)
      gl();
    else
      throw curr_lex;
  } else
    throw curr_lex;

}

#include "executer.cpp"

// int main(){
//   Parser pars("test.t");
//   try{
//     pars.analyze();
//   } catch (const char *tstr ){
//     cout << "Error: " << tstr << endl;
//   }
// }


int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << argv[0];
    cerr << "Specify the source file\n";
    throw "Specify the source file";
  }

  cout << "Content-type: text/html\n\n";
  cout << "<html><body><b>THIS IS THE OUTPUT OF YOUR PROGRAM:</b>";
  cout << "<p>";
  try {
    cerr << "Interpreter says :" << argv[1] << endl;
    Interpretator I(argv[1]);
    I.interpretation();
    cout << "</p></body></html>";
    return 0;
  } catch (char c) {
    cerr << "unexpected symbol " << c << endl;
    return 1;
  } catch (Lex l) {
    cerr << "unexpected lexeme";
    cerr << l;
    return 1;
  } catch (const char *source) {
    cerr << source << endl;
    cerr << "Error source";
    return 1;
  }
}
