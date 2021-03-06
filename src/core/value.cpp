/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include <QString>
#include "value.h"

namespace evoplex {

Value::Value() : m_type(INVALID)
{
}

Value::Value(const Value& value) : m_type(value.type())
{
    if (m_type == INT) m_data.i = value.toInt();
    else if (m_type == DOUBLE) m_data.d = value.toDouble();
    else if (m_type == BOOL) m_data.b = value.toBool();
    else if (m_type == CHAR) m_data.c = value.toChar();
    else if (m_type == STRING) m_data.s = qstrdup(value.toString());
    else if (m_type != INVALID) qFatal("non-existent type");
}

Value::Value(bool value) : m_type(BOOL)
{
    m_data.b = value;
}

Value::Value(std::vector<bool>::reference value) : m_type(BOOL)
{
    m_data.b = value.operator bool();
}

Value::Value(char value) : m_type(CHAR)
{
    m_data.c = value;
}

Value::Value(double value) : m_type(DOUBLE)
{
    m_data.d = value;
}

Value::Value(int value) : m_type(INT)
{
    m_data.i = value;
}

Value::Value(const char* value) : m_type(STRING)
{
    // considering that Value(const QString& value) needs to call a new char[],
    // which must be deleted in this class, for consistency, let's do the same
    // here as well, and we can safely use delete[] whenever it's a STRING
    m_data.s = qstrdup(value);
}

// converts the QString to a char*
Value::Value(const QString& value) : m_type(STRING)
{
    QByteArray text = value.toUtf8();
    m_data.s = qstrdup(text.constData());
}

Value::~Value()
{
    if (m_type == STRING) {
        delete [] m_data.s;
    }
}

QString Value::toQString(char format, int precision) const
{
    switch (m_type) {
    case INT: return QString::number(m_data.i);
    case DOUBLE: return QString::number(m_data.d, format, precision);
    case BOOL: return QString::number(m_data.b);
    case CHAR: return QString(m_data.c);
    case STRING: return QString::fromUtf8(m_data.s);
    case INVALID: return QString();
    }
    throw std::invalid_argument("invalid type of Value");
}

Value& Value::operator=(const Value& v)
{
    if (this != &v) { // check for self-assignment
        if (m_type == STRING) { delete [] m_data.s; }
        m_type = v.m_type;
        switch (m_type) {
        case INT: m_data.i = v.m_data.i; break;
        case DOUBLE: m_data.d = v.m_data.d; break;
        case BOOL: m_data.b = v.m_data.b; break;
        case CHAR: m_data.c = v.m_data.c; break;
        case STRING: m_data.s = qstrdup(v.m_data.s); break;
        case INVALID: /* nothing-to-copy */ break;
        }
    }
    return *this;
}

bool Value::operator==(const Value& v) const
{
    if (m_type != v.m_type) {
        return false;
    }

    switch (m_type) {
    case INT: return m_data.i == v.m_data.i;
    case DOUBLE: return qFuzzyCompare(m_data.d, v.m_data.d);
    case BOOL: return m_data.b == v.m_data.b;
    case CHAR: return m_data.c == v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) == 0;
    case INVALID: return m_type == v.m_type;
    }
    throw std::invalid_argument("invalid type of Value");
}

bool Value::operator!=(const Value& v) const
{
    if (m_type != v.m_type) {
        return true;
    }

    switch (m_type) {
    case INT: return m_data.i != v.m_data.i;
    case DOUBLE: return !qFuzzyCompare(m_data.d, v.m_data.d);
    case BOOL: return m_data.b != v.m_data.b;
    case CHAR: return m_data.c != v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) != 0;
    case INVALID: return m_type != v.m_type;
    }
    throw std::invalid_argument("invalid type of Value");
}

bool Value::operator<(const Value& v) const
{
    if (m_type != v.m_type) {
        throw std::invalid_argument("[operator<] type should be the same");
    }

    switch (m_type) {
    case INT: return m_data.i < v.m_data.i;
    case DOUBLE: return m_data.d < v.m_data.d;
    case BOOL: return m_data.b < v.m_data.b;
    case CHAR: return m_data.c < v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) < 0;
    default: throw std::invalid_argument("invalid type of Value");
    }
}

bool Value::operator>(const Value& v) const
{
    if (m_type != v.m_type) {
        throw std::invalid_argument("[operator>] type should be the same");
    }

    switch (m_type) {
    case INT: return m_data.i > v.m_data.i;
    case DOUBLE: return m_data.d > v.m_data.d;
    case BOOL: return m_data.b > v.m_data.b;
    case CHAR: return m_data.c > v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) > 0;
    default: throw std::invalid_argument("invalid type of Value");
    }
}

bool Value::operator<=(const Value& v) const
{
    if (m_type != v.m_type) {
        throw std::invalid_argument("[operator<=] type should be the same");
    }

    switch (m_type) {
    case INT: return m_data.i <= v.m_data.i;
    case DOUBLE: return m_data.d <= v.m_data.d;
    case BOOL: return m_data.b <= v.m_data.b;
    case CHAR: return m_data.c <= v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) <= 0;
    default: throw std::invalid_argument("invalid type of Value");
    }
}

bool Value::operator>=(const Value& v) const
{
    if (m_type != v.m_type) {
        throw std::invalid_argument("[operator>=] type should be the same");
    }

    switch (m_type) {
    case INT: return m_data.i >= v.m_data.i;
    case DOUBLE: return m_data.d >= v.m_data.d;
    case BOOL: return m_data.b >= v.m_data.b;
    case CHAR: return m_data.c >= v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) >= 0;
    default: throw std::invalid_argument("invalid type of Value");
    }
}

std::logic_error Value::throwError() const
{
    switch (m_type) {
    case INT: return std::logic_error("It is an integer. You must use toInt()");
    case DOUBLE: return std::logic_error("It is a double. You must use toDouble()");
    case BOOL: return std::logic_error("It is a boolean. You must use toBool()");
    case CHAR: return std::logic_error("It is a char. You must use toChar()");
    case STRING: return std::logic_error("It is a string. You must use toString()");
    default: return std::logic_error("Value is invalid. Data cannot be retrieved.");
    }
}

} // evoplex
