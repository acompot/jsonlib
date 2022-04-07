#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include<type_traits>
#include<variant>
#include<utility>
#include <initializer_list>
#include <list>
namespace json {

class Node;

using Number = std::variant<int, double>;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using JSONtypes = std::variant<std::nullptr_t,  bool, int, double, std::string,Array,Dict>;
// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:

    Node() =default;    
    Node(std::vector<Node> array)                          :jtypes_(std::move(array)){}
    Node(Dict map)                                         :jtypes_(std::move(map)){}
    Node(std::string s)                                    :jtypes_(std::move(s)){}
    Node(int s)                                            :jtypes_(std::move(s)){}
    Node(double  s)                                        :jtypes_(std::move(s)){}
    Node(bool  s)                                          :jtypes_(std::move(s)){}
    Node(std::nullptr_t s)                                 :jtypes_(std::move(s)){}
//    template<typename T>
//    Node(T t) :jtypes_(std::move(t)){}

    void PrintNode(std::ostream &output) const;
    bool IsInt() const;
    bool IsNull() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsString() const;
    bool IsBool() const;
    bool IsArray() const;
    bool IsMap() const;

     double AsDouble() const;
     int  AsInt() const;
     const std::string& AsString() const;
     bool AsBool() const;
     const Array& AsArray() const;
     const Dict &AsMap() const;
    bool operator==(const Node& rhs) const{ return jtypes_ ==  rhs.jtypes_;}
    bool operator !=(const Node& rhs) const{ return jtypes_ !=  rhs.jtypes_;}
private:
    JSONtypes jtypes_;

    struct Printstruct {
        std::ostream &out;
        void PrintString(std::string s,std::ostream &out) const{
            out << "\"";
            for ( auto ch : s ){
              switch (ch)
              {
                case '\"':out << "\\\"";break;
                case '\\':out << "\\\\";break;
                case '\n':out << "\\n";break;
                case '\r':out << "\\r";break;
                case '\t':out << "\\t";break;
                default:out << ch;
              }
            }
            out << "\"";
        }
        void operator()(std::nullptr_t) const {
            out << "null";
        }
        void operator()(double d) const { out << d;}
        void operator()(int i) const { out << i; }
        void operator()(std::string s) const {
          PrintString(s,out);
        }
        void operator()(bool b) const { out << std::boolalpha <<b;}
        void operator()(Array arr) const {
            bool f = false;
            out << "[";
            for (auto a : arr ){
                 if (!f) f = true;
                 else  out << ", ";
                 a.PrintNode(out);
            }
             out << "]";

        }
        void operator()(Dict dic) const {
            bool f = false;
            out << "{ ";
            for (auto [key,value] : dic){
                if(!f) f =true;
                else  out << ", ";
                PrintString(key,out);
                out << ": ";
                value.PrintNode(out);
            }
            out << "}";


        }

    };


};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;
    bool operator==(const Document& rhs) const{ return root_ ==  rhs.root_;}
    bool operator !=(const Document& rhs) const{ return !(root_ ==  rhs.root_);}
private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json
