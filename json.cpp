#include "json.h"
struct PrintHelper;
using namespace std;
#include <cassert>
namespace json {

namespace {

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
  Array result;
  for (char c; input >> c &&  c != ']';) {
        if (c != ',') {
           input.putback(c);
        }
        result.push_back(LoadNode(input));
  }
  if (!input) {
      throw ParsingError("Unexpected end"s);
  }
  return Node(move(result));

}

Node LoadString(istream& input) {
    string tmp;
    char c;
    while(input){
        input.get(c);
        if (c=='"') break;
        else{
          if (c=='\n' || c=='\r') throw ParsingError("Unexpected end"s);
          if (c=='\\'){
            //Проверка на конец строки
            if (input.peek() == 0xff)  throw ParsingError("Unexpected end"s);
            else{
              input.get(c);
              switch (c) {
              case '"':tmp+='"';break;
              case '\\':tmp+='\\';break;
              case 'r':tmp+='\r';break;
              case 'n':tmp+='\n';break;
              case 't':tmp+='\t';break;
              default:throw ParsingError("unssuported escape"s);break;
              }
           }
        }
          else tmp+=c;
        }
    }
    if (input.eof())throw ParsingError("Unexpected end"s);
   return Node(move(tmp));
}

Node LoadDict(istream& input) {
  Dict result;
  for (char c; input >> c && c != '}';) {
    if (c == ',') {
       input >> c;
    }
    string key = LoadString(input).AsString();
    input >> c;
    result.insert({move(key), LoadNode(input)});
  }
  if (!input) {
      throw ParsingError("Unexpected end"s);
  }
  return Node(move(result));
}

Number LoadNumber(istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadBoolNUll(istream& input) {

  char c;
  char buff[10];
  while( input.get(c)){
      if (c=='n'){
          input.putback(c);
          input.get(buff,5);
          string z(buff);
          if (z == "null") return Node(move(nullptr));
          else   throw ParsingError("boxwood"s);
          break;
      } else if( c=='t'){
          input.putback(c);

          input.get(buff,5);
          string z(buff);
          if (z == "true") return Node(move(true));
          else   throw ParsingError("boxwood"s);
          break;
      } else if (c== 'f'){
          input.putback(c);
          input.get(buff,6);
          string z(buff);
          if (z == "false") return Node(move(false));
          else   throw ParsingError("boxwood"s);
          break;
      }

  }
   throw ParsingError("boxwood"s);
//  else  if (tmp == "true") return Node(move(true));
//  else  if (tmp == "false") return Node(move(false));
//  else   throw ParsingError("boxwood"s);
}

Node LoadNode(istream& input) {
  char c;
  input >> c;

  if ( isalpha(c)){
     input.putback(c);
     return LoadBoolNUll(input);
  }
  else if (c == '[') {
    return LoadArray(input);
  } else if (c == '{') {
    return LoadDict(input);
  } else if (c == '"') {
    return LoadString(input);
  } else {
    input.putback(c);
    auto z   = LoadNumber(input);
    if (std::holds_alternative<int>(z)) return Node(std::move(std::get<int>(z)));
    else if (std::holds_alternative<double>(z)) return Node(std::move(std::get<double>(z)));
    else  throw logic_error("Failed to convert JSON"s);
  }
  throw ParsingError("Error istrem"s);

}
}  // namespace



int Node::AsInt() const {
  if(IsInt())   return std::get<int>(jtypes_);
  else {
       throw logic_error("Failed int");
  }
}

double Node::AsDouble() const {
 if (IsPureDouble()) return (std::get<double>(jtypes_));
 else if (IsInt()) return static_cast<double>(std::get<int>(jtypes_));
 else throw logic_error("Failed double");
}

const string& Node::AsString() const {
   if(IsString()) return std::get<std::string>(jtypes_);
   else throw logic_error("Failed string");
}

const Array& Node::AsArray() const {
    if (IsArray())return std::get<Array>(jtypes_);
    else  throw logic_error("Failed Array");
}


bool Node::AsBool() const {
    if (IsBool() )return std::get<bool>(jtypes_);
     else throw logic_error("Failed bool");
}

const Dict &Node::AsMap() const {
    if (IsMap())return std::get<Dict>(jtypes_);
    else throw logic_error("Failed Map");
}

bool Node::IsInt() const  {return std::holds_alternative<int>(jtypes_);}

bool Node::IsDouble()const {return ( std::holds_alternative<double>(jtypes_) || IsInt());}

bool Node::IsNull() const {return std::holds_alternative<std::nullptr_t>(jtypes_);}

bool Node::IsPureDouble() const {return std::holds_alternative<double>(jtypes_);}

bool Node::IsString() const {return std::holds_alternative<std::string>(jtypes_);}

bool Node::IsBool() const {return std::holds_alternative<bool>(jtypes_);}

bool Node::IsArray() const {return std::holds_alternative<Array>(jtypes_);}

bool Node::IsMap() const {return std::holds_alternative<Dict>(jtypes_);}

void Node::PrintNode(std::ostream& output) const{std::visit(Printstruct{output}, jtypes_);}

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, std::ostream& output) {
    Node t = doc.GetRoot();
    t.PrintNode(output);

    // Реализуйте функцию самостоятельно
}

}  // namespace json
