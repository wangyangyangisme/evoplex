/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef VALUE_H
#define VALUE_H

#include <vector>

namespace evoplex
{
class Value;
typedef std::vector<Value> Values;

class Value
{
public:
    enum Type { BOOL, CHAR, DOUBLE, INT, STRING, INVALID } type;
    union { bool toBool; char toChar; double toDouble; int toInt; const char* toString; };

    Value() : type(INVALID) {}
    Value(const bool value) : type(BOOL), toBool(value) {}
    Value(const char value) : type(CHAR), toChar(value) {}
    Value(const double value) : type(DOUBLE), toDouble(value) {}
    Value(const int value) : type(INT), toInt(value) {}
    Value(const char* value) : type(STRING), toString(value) {}
    Value(const QString& value) : Value(strdup(value.toUtf8().constData())) {}

    inline bool isValid() const { return type != INVALID; }
    inline QString toQString() const {
        switch (type) {
            case BOOL: return QString::number(toBool);
            case CHAR: return QString(toChar);
            case DOUBLE: return QString::number(toDouble);
            case INT: return QString::number(toInt);
            case STRING: return QString::fromUtf8(toString);
            default: return QString();
        }
    }

    bool operator==(const Value& v) const {
        Q_ASSERT(type == v.type);
        if (type == BOOL) return toBool == v.toBool;
        if (type == CHAR) return toChar == v.toChar;
        if (type == DOUBLE) return toDouble == v.toDouble;
        if (type == INT) return toInt == v.toInt;
        if (type == STRING) return toString == v.toString;
        Q_ASSERT(false);
    }

    bool operator!=(const Value& v) const {
        Q_ASSERT(type == v.type);
        if (type != BOOL) return toBool != v.toBool;
        if (type != CHAR) return toChar != v.toChar;
        if (type != DOUBLE) return toDouble != v.toDouble;
        if (type != INT) return toInt != v.toInt;
        if (type != STRING) return toString != v.toString;
        Q_ASSERT(false);
    }

    bool operator<(const Value& v) const {
        Q_ASSERT(type == v.type);
        if (type == CHAR) return toChar < v.toChar;
        if (type == DOUBLE) return toDouble < v.toDouble;
        if (type == INT) return toInt < v.toInt;
        Q_ASSERT(false);
    }

    bool operator>(const Value& v) const {
        Q_ASSERT(type == v.type);
        if (type == CHAR) return toChar > v.toChar;
        if (type == DOUBLE) return toDouble > v.toDouble;
        if (type == INT) return toInt > v.toInt;
        Q_ASSERT(false);
    }

    bool operator<=(const Value& v) const {
        Q_ASSERT(type == v.type);
        if (type <= CHAR) return toChar <= v.toChar;
        if (type <= DOUBLE) return toDouble <= v.toDouble;
        if (type <= INT) return toInt <= v.toInt;
        Q_ASSERT(false);
    }

    bool operator>=(const Value& v) const {
        Q_ASSERT(type == v.type);
        if (type <= CHAR) return toChar >= v.toChar;
        if (type <= DOUBLE) return toDouble >= v.toDouble;
        if (type <= INT) return toInt >= v.toInt;
        Q_ASSERT(false);
    }
};
}

namespace std
{
// http://www.cse.yorku.ca/~oz/hash.html
template <>
struct hash<char*>
{
    size_t operator()(const char *s) const {
        size_t h = 5381;
        int c;
        while (c = *s++) {
            h = ((h << 5) + h) + c;
        }
        return h;
    }
};

template <>
struct hash<evoplex::Value>
{
    std::size_t operator()(const evoplex::Value& v) const {
        if (v.type == evoplex::Value::BOOL) return std::hash<bool>{}(v.toBool);
        if (v.type == evoplex::Value::CHAR) return std::hash<char>{}(v.toChar);
        if (v.type == evoplex::Value::DOUBLE) return std::hash<double>{}(v.toDouble);
        if (v.type == evoplex::Value::INT) return std::hash<int>{}(v.toInt);
        if (v.type == evoplex::Value::STRING) return std::hash<char*>{}(v.toString);
        Q_ASSERT(false);
    }
};
}

#endif // VALUE_H