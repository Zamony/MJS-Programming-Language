class Executer {
  Lex pc_el;
 public:
  void execute(Poliz& prog);
};

void Executer::execute(Poliz& prog) {
  Stack<int, 100> args;
  int i, j, index = 0, size = prog.get_free();
  while (index < size) {
    pc_el = prog[index];
    switch (pc_el.get_type()) {
      case LEX_TRUE:
      case LEX_FALSE:
      case LEX_NUM:
      case POLIZ_ADDRESS:
      case POLIZ_LABEL:
        args.push(pc_el.get_value());
        break;
      case LEX_ID:
        i = pc_el.get_value();
        if (TID[i].get_assign()) {
          args.push(TID[i].get_value());
          break;
        } else
          throw "POLIZ: indefinite identifier";
      case LEX_NOT:
        args.push(!args.pop());
        break;
      case LEX_OR:
        i = args.pop();
        args.push(args.pop() || i);
        break;
      case LEX_AND:
        i = args.pop();
        args.push(args.pop() && i);
        break;
      case POLIZ_GO:
        index = args.pop() - 1;
        break;
      case POLIZ_FGO:
        i = args.pop();
        if (!args.pop()) index = i - 1;
        break;
      case LEX_WRITE:
        cout << args.pop() << endl;
        break;
      case LEX_READ: {
        int k;
        i = args.pop();
        if (TID[i].get_type() == LEX_INT) {
          cout << "Input int value for";
          cout << TID[i].get_name() << endl;
          cin >> k;
        } else {
          char j[20];
          rep:
              cout << "Input boolean value";
              cout << "(true or false) for";
              cout << TID[i].get_name() << endl;
              cin >> j;
              if (!strcmp(j, "true"))
                k = 1;
              else if (!strcmp(j, "false"))
                k = 0;
              else {
                cout << "Error in input:true/false";
                cout << endl;
                goto rep;
              }
        }
        TID[i].put_value(k);
        TID[i].put_assign();
        break;
      }
      case LEX_PLUS:
        args.push(args.pop() + args.pop());
        break;
      case LEX_TIMES:
        args.push(args.pop() * args.pop());
        break;
      case LEX_MINUS:
        i = args.pop();
        args.push(args.pop() - i);
        break;
      case LEX_SLASH:
        i = args.pop();
        if (i != 0) {
          args.push(args.pop() / i);
          break;
        } else
          throw "POLIZ:divide by zero";
      case LEX_EQ:
        args.push(args.pop() == args.pop());
        break;
      case LEX_LSS:
        i = args.pop();
        args.push(args.pop() < i);
        break;
      case LEX_GTR:
        i = args.pop();
        args.push(args.pop() > i);
        break;
      case LEX_LEQ:
        i = args.pop();
        args.push(args.pop() <= i);
        break;
      case LEX_GEQ:
        i = args.pop();
        args.push(args.pop() >= i);
        break;
      case LEX_NEQ:
        i = args.pop();
        args.push(args.pop() != i);
        break;
      case LEX_ASSIGN:
        i = args.pop();
        j = args.pop();
        TID[j].put_value(i);
        TID[j].put_assign();
        break;
      default:
        throw "POLIZ: unexpected elem ";
  }//end of switch
  ++index;
}; // end of while
cout << "OK. Execution finished" << endl;
}

class Interpretator {
  Parser pars;
  Executer E;

 public:
  Interpretator(char* program) : pars(program){};
  void interpretation();
};
void Interpretator::interpretation() {
  pars.analyze();
  E.execute(pars.prog);
}
