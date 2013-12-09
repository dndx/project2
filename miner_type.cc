#include <cctype>
#include "miner_type.h"

Value::Value() : value_type(INVALID) {}
Value::Value(const char *s) : string_value(s), value_type(STRING) {}
Value::Value(const std::string &s) : string_value(s), value_type(STRING) {}
Value::Value(int s) : integer_value(s), value_type(INTEGER) {}
Value::Value(const std::pair<int, int> &s) : range_value(s), value_type(RANGE) {}
Value::Value(const std::array<int, 3> &s) : triple_value(s), value_type(TRIPLE) {}
Value::Value(const List &s) : list_value(s), value_type(LIST) {}
Value::Value(const Struct &s) : struct_value(s), value_type(STRUCT) {}

Value::Value(const Value &v)
{
    switch (v.value_type)
    {
        case STRING:
            string_value = v.string_value;
            value_type = STRING;
            break;
        case INTEGER:
            integer_value = v.integer_value;
            value_type = INTEGER;
            break;
        case RANGE:
            range_value = v.range_value;
            value_type = RANGE;
            break;
        case TRIPLE:
            triple_value = v.triple_value;
            value_type = TRIPLE;
            break;
        case LIST:
            list_value = v.list_value;
            value_type = LIST;
            break;
        case STRUCT:
            struct_value = v.struct_value;
            value_type = STRUCT;
            break;
        case INVALID:
            value_type = INVALID;
            break;
    }
}

Value::Value(Value &&v)
{
    switch (v.value_type)
    {
        case STRING:
            string_value = std::move(v.string_value);
            value_type = STRING;
            break;
        case INTEGER:
            integer_value = v.integer_value;
            value_type = INTEGER;
            break;
        case RANGE:
            range_value = v.range_value;
            value_type = RANGE;
            break;
        case TRIPLE:
            triple_value = v.triple_value;
            value_type = TRIPLE;
            break;
        case LIST:
            list_value = std::move(v.list_value);
            value_type = LIST;
            break;
        case STRUCT:
            struct_value = std::move(v.struct_value);
            value_type = STRUCT;
            break;
        case INVALID:
            value_type = INVALID;
            break;
    }
}

Value &Value::operator=(Value &&v)
{
    switch (v.value_type)
    {
        case STRING:
            string_value = std::move(v.string_value);
            value_type = STRING;
            break;
        case INTEGER:
            integer_value = v.integer_value;
            value_type = INTEGER;
            break;
        case RANGE:
            range_value = v.range_value;
            value_type = RANGE;
            break;
        case TRIPLE:
            triple_value = v.triple_value;
            value_type = TRIPLE;
            break;
        case LIST:
            list_value = std::move(v.list_value);
            value_type = LIST;
            break;
        case STRUCT:
            struct_value = std::move(v.struct_value);
            value_type = STRUCT;
            break;
        case INVALID:
            value_type = INVALID;
            break;
    }

    return *this;
}

Value &Value::operator=(const Value &v)
{
    switch (v.value_type)
    {
        case STRING:
            string_value = v.string_value;
            value_type = STRING;
            break;
        case INTEGER:
            integer_value = v.integer_value;
            value_type = INTEGER;
            break;
        case RANGE:
            range_value = v.range_value;
            value_type = RANGE;
            break;
        case TRIPLE:
            triple_value = v.triple_value;
            value_type = TRIPLE;
            break;
        case LIST:
            list_value = v.list_value;
            value_type = LIST;
            break;
        case STRUCT:
            struct_value = v.struct_value;
            value_type = STRUCT;
            break;
        case INVALID:
            value_type = INVALID;
            break;
    }

    return *this;
}

ValueType Value::get_type() const noexcept {return value_type; }

List::List() = default;
List::List(const List &l) : v(l.v) {}
List::List(List &&l) : v(std::move(l.v)) {}
List &List::operator=(const List &l)
{
    v = l.v;
    return *this;
}

List &List::operator=(List &&l)
{
    v = std::move(l.v);
    return *this;
}

Value& List::operator[](size_t i) {return v[i]; }
size_t List::size() const {return v.size(); }
void List::push_back(const Value &value) {v.push_back(value); }

Struct::Struct() = default;
Struct::Struct(const Struct &s) : m(s.m) {}
Struct::Struct(Struct &&s) : m(std::move(s.m)) {}
Struct &Struct::operator=(const Struct &l)
{
    m = l.m;
    return *this;
}

Struct &Struct::operator=(Struct &&l)
{
    m = std::move(l.m);
    return *this;
}

std::map<std::string, Value>::const_iterator Struct::cbegin() const noexcept
{
    return m.cbegin();
}

std::map<std::string, Value>::const_iterator Struct::cend() const noexcept
{
    return m.cend();
}

Value& Struct::operator[](const std::string key) {return m[key]; }
size_t Struct::count (const std::map<std::string, Value>::key_type& k) const {return m.count(k); }
size_t Struct::size() const {return m.size(); }

using namespace std;

/*
 * Output struct in Dr. Miner's format
 */
std::ostream &operator<<(std::ostream &out, const Value &v)
{
    switch (v.get_type())
    {
        case STRING:
            out << '"' << (string) v << '"';
            break;
        case INTEGER:
            out << (int) v;
            break;
        case RANGE:
        {
            pair<int, int> p = v;
            out << p.first << ".." << p.second;
            break;
        }
        case TRIPLE:
        {
            array<int, 3> a = v;
            out << '(' << a[0] << ", " << a[1] << ", " << a[2] << ')';
            break;
        }
        case LIST:
        {
            List l = (List) v;

            for (decltype(l.size()) i = 0; i < l.size(); i++) // C++11 feature
            {
                pair<int, int> p = l[i];
                if (i)
                    out << ", (" << p.first << ", " << p.second << ")";
                else
                    out << '(' << p.first << ", " << p.second << ")";
            }
            break;
        }
        case STRUCT: // Struct, output assignment by assignment
        {
            Struct s = (Struct) v;

            out << '{' << endl;
            for (auto i = s.cbegin(); i != s.cend(); i++)
            {
                out << (*i).first << " = " << (*i).second << ';' << endl;
            }
            out << '}';
            break;
        }
        case INVALID:
            out << "<Invalid Value>";
            break;
    }

    return out; // return ostream& for chain output
}

