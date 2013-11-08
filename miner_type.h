#ifndef MINER_TYPE_H
#define MINER_TYPE_H

#include <string>
#include <utility>
#include <array>
#include <vector>
#include <memory>
#include <map>
#include <istream>

enum ValueType {STRING, INTEGER, RANGE, TRIPLE, LIST, STRUCT, INVALID};

class Value;

class List {
    public:
        List();
        List(const List &l);
        List(List &&l);

        Value& operator[](size_t i);
        size_t size() const;
        void push_back(const Value &value);
        List &operator=(const List &l);
        List &operator=(List &&l);

    private:
        std::vector<Value> v;
};

class Struct {
    public:
        Struct();
        Struct(const Struct &s);
        Struct(Struct &&s);

        Value& operator[](const std::string key);
        size_t count(const std::map<std::string, Value>::key_type& k) const;
        size_t size() const;
        Struct &operator=(const Struct &l);
        Struct &operator=(Struct &&l);
        std::map<std::string, Value>::const_iterator cbegin() const noexcept;
        std::map<std::string, Value>::const_iterator cend() const noexcept;

    private:
        std::map<std::string, Value> m;
};

class Value {
    public:
        operator std::string() const {return string_value; }
        operator int() const {return integer_value; }
        operator std::pair<int, int>() const {return range_value; }
        operator std::array<int, 3>() const {return triple_value; }
        operator List() const {return list_value; }
        operator Struct() const {return struct_value; }

    private:
        std::string string_value;
        int integer_value;
        std::pair<int, int> range_value;
        std::array<int, 3> triple_value;
        List list_value;
        Struct struct_value;

        ValueType value_type;

    public:
        Value();
        Value(const char *s);
        Value(const std::string &s);
        Value(int s);
        Value(const std::pair<int, int> &s);
        Value(const std::array<int, 3> &s);
        Value(const List &s);
        Value(const Struct &s);
        Value(const Value &s);
        Value(Value &&s);
        Value &operator=(const Value &v);
        Value &operator=(Value &&v);

        ValueType get_type() const noexcept;

};

std::ostream &operator<<(std::ostream &out, const Value &v);

std::string preprocess_file(std::istream &fin);

Value parse_string(std::string::const_iterator begin, std::string::const_iterator end);

#endif /* !MINER_TYPE_H */
