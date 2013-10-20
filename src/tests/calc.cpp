// test calculator


#include <iostream>
#include <string>
#include <map>
#include <cctype>
#include <sstream>


using namespace std;


class Calculator {
  typedef map< string, double > Map;
  typedef Map::value_type       Map_type;
  
  enum Token_value {
    NAME,        NUMBER,    END,
    MUL = '*',   DIV = '/', PLUS = '+',  MINUS = '-',
    PRINT = ';', LP = '(',  RP = ')',    ASSIGN = '=',
    PT = '.',    NL = '\n'
  };


public:
  struct Main_args {
    int argc;
    char **argv;

    Main_args(int argc = 0, char **argv = 0) : argc(argc), argv(argv) {}

    Main_args& operator= (const Main_args &r) {
      if (this != &r) {
        argc = r.argc;
        argv = r.argv;
      }
      return *this;
    }

  };

  Calculator(const Main_args &ma) : m_main_args(ma), m_cur_tok(PRINT) {
    switch (m_main_args.argc) {
      case 1:
        m_input = &cin;
        break;
      case 2:
        m_input = new istringstream(m_main_args.argv[1]);
        break;
      default: error("слишком много параметров");
    }

    m_table.insert(Map_type("pi", 3.1415926535897932385));
    m_table.insert(Map_type("e",  2.7182818284590452354));

    while (m_input) {
      get_token();

      if (m_cur_tok == END) {
        break;
      }

      if (m_cur_tok != PRINT) {
        cout << expr(false) << "\n";
      }
    }

    if (m_input != &cin) {
      delete m_input;
    }
  }

  int get_error() {
    return m_no_of_errors;
  }

private:
  int error(const string &s) {
    m_no_of_errors++;
    cerr << "ошибка: " << s << endl;
    return 1;
  }

  Token_value get_token() {
    char ch = 0;

    // пропустить все символы разделители кроме \n
    do { 
      if ( ! m_input->get(ch)) {
        return m_cur_tok = END;
      }
    } while (ch != NL && isspace(ch));

    switch (ch) {
      case 0:
        return m_cur_tok = END;
      case PRINT:
      case NL:
        return m_cur_tok = PRINT;
      case MUL: case DIV: case PLUS: case MINUS: case LP: case RP: case ASSIGN:
        return m_cur_tok = Token_value(ch);
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case PT:
        m_input->putback(ch); // поместить `ch` обратно в ввод
        *m_input >> m_num_val;
        return m_cur_tok = NUMBER;
      default:
        if (isalpha(ch)) {
          m_str_val = ch;
 
          while (m_input->get(ch) && isalnum(ch)) {
            m_str_val.push_back(ch);
          }
          m_input->putback(ch);
          return m_cur_tok = NAME;
        }
        error("нерпавильная лекскма");
        return m_cur_tok = PRINT;
    }
  }

  // первичное выражение
  double prim(bool get) {
    if (get) {
      get_token();
    }

    switch (m_cur_tok) {
      case NUMBER: { // константа с плавающей точкой
        double v = m_num_val;
        get_token();
        return v; 
      }
      case NAME: {
        double &v = m_table[m_str_val];

        if (get_token() == ASSIGN) {
          v = expr(true);
        }
        return v;
      }
      case MINUS: // унарный минус
        return -prim(true);
      case LP: {
        double e = expr(true);

        if (m_cur_tok != RP) {
          return error("ожидалось )");
        }
        get_token(); // пропустить скобки
        return e;
      }
      default: return error("ожидалось первичное выражение");
    }
  }
  
  // mul & div
  double term(bool get) {
    double left = prim(get);

    for (;;) {
      switch (m_cur_tok) {
        case MUL: left *= prim(true); break;
        case DIV:
          if (double d = prim(true)) {
            left /= prim(true);
            break;
          }
          return error("Деление на 0.");
        default: return left;
      }
    }
  }

  // plus & minus
  double expr(bool get) {
    double left = term(get);

    for (;;) {
      switch (m_cur_tok) {
        case PLUS:  left += term(true); break;
        case MINUS: left -= term(true); break;
        default: return left;
      }
    }
  }

  Main_args m_main_args;
  istream *m_input;
  Map m_table;
  Token_value m_cur_tok;
  double m_num_val; // чесленное значение
  string m_str_val; // строковое значение
  int m_no_of_errors;

};


int main(int argc, char **argv) {
  Calculator calc(Calculator::Main_args(argc, argv));
  return calc.get_error();
}
