#ifndef PARSER_TABLES_HPP_SENTRY
#define PARSER_TABLES_HPP_SENTRY

class PolizItem;

#include "String.hpp"
#include "DynamicTable.hpp"
#include "TableAccessException.hpp"

enum ScriptKeywords {
    SCR_KEYWORD_SET,
    SCR_KEYWORD_ARRAY,
    SCR_KEYWORD_GOTO,
    SCR_KEYWORD_IF,
    SCR_KEYWORD_ELSE,
    SCR_KEYWORD_WHILE,
    SCR_KEYWORD_PRINT,
    SCR_KEYWORD_BUY,
    SCR_KEYWORD_SELL,
    SCR_KEYWORD_MAKE,
    SCR_KEYWORD_BUILD,
    SCR_KEYWORD_TURN,
    SCR_KEYWORD_JOIN
};

enum ScriptGameFunctions {
    SCR_FUNC_IS_WIN,
    SCR_FUNC_GET_WINNERS,
    SCR_FUNC_MY_NICK,
    SCR_FUNC_GET_PLAYERS,
    SCR_FUNC_GET_MARKET_RAWS,
    SCR_FUNC_GET_RAW_PRICE,
    SCR_FUNC_GET_MARKET_PRODUCTIONS,
    SCR_FUNC_GET_PRODUCTION_PRICE,
    SCR_FUNC_GET_MONEY,
    SCR_FUNC_GET_RAWS,
    SCR_FUNC_GET_PRODUCTIONS,
    SCR_FUNC_GET_FACTORIES
};

template <class T>
class ValueElemGeneric : public NamedElem {
    bool defined;
    T value;

public:
    ValueElemGeneric<T>(char* aName)
        : NamedElem(aName),
        defined(false),
        value(0)
    {}

    bool isDefined()
    {
        return defined;
    }

    void setValue(T aValue)
    {
        defined = true;
        value = aValue;
    }

    T getValue()
    {
        return value;
    }
};

typedef NamedElem StringElem;
typedef ValueElemGeneric<int> VariableElem;
typedef ValueElemGeneric<PolizItem*> LabelElem;

// TODO
class ArrayAccessException : public Exception {
public:
    ArrayAccessException()
        : Exception(__FILE__, __LINE__)
    {}

    const char* toString() const
    {
        return "ArrayAccessException()";
    }
};

class ArrayElem : public NamedElem {
    int size;
    int* p;

public:
    ArrayElem(char* aName)
        : NamedElem(aName),
        size(-1),
        p(0)
    {}

    bool isDefined()
    {
        return (size != -1);
    }

    void setSize(int aSize)
    {
        if (aSize <= 0) {
            throw ArrayAccessException();
        }

        if (isDefined()) {
            throw ArrayAccessException();
        }

        size = aSize;
        p = new int[size];

        for (int i = 0; i < size; ++i) {
            p[i] = 0;
        }
    }

    int getSize()
    {
        if (! isDefined()) {
            throw ArrayAccessException();
        }

        return size;
    }

    int getElementValue(int index)
    {
        if (index < 0 || index >= size) {
            throw ArrayAccessException();
        }

        if (! isDefined()) {
            throw ArrayAccessException();
        }

        return p[index];
    }

    void setElementValue(int index, int value)
    {
        if (index < 0 || index >= size) {
            throw ArrayAccessException();
        }

        if (! isDefined()) {
            throw ArrayAccessException();
        }

        p[index] = value;
    }
};

class ParserTables {
    static const char* keywordStrings[];
    static const char* functionStrings[];
    DynamicTable strings;
    DynamicTable variables;
    DynamicTable arrays;
    DynamicTable labels;

public:
    ParserTables();

    // Throw exception, if keyword is unknown.
    ScriptKeywords getKeywordType(
        const char* str) const
        throw (TableAccessException);

    ScriptGameFunctions getFunctionType(
        const char* str) const
        throw (TableAccessException);

    bool functionHasNoArguments(
        ScriptGameFunctions) const
        throw ();

    // Make string entry, if not found.
    int getStringKey(const char* name)
        throw ();
    // Throw exception, if not found.
    char* getStringValue(int key) const
        throw (TableAccessException);

    // Argument def mean: "must be defined".
    // If def is true:
    // * Throw exception, if not found.
    // If def is false:
    // * Make variable entry, if not found.
    int getVariableKey(const char* name, bool def)
        throw (TableAccessException);
    // Throw exception, if not found or not defined.
    int getVariableValue(int key) const
        throw (TableAccessException);
    // Throw exception, if not found.
    void setVariableValue(int key, int value)
        throw (TableAccessException);

    // Argument def mean: "must be defined".
    // If def is true:
    // * Throw exception, if not found.
    // (TODO: and not defined?)
    // If def is false:
    // * Make array entry, if not found.
    // * Throw exception, if found.
    int getArrayKey(const char* name, bool def)
        throw (TableAccessException);
    // Throw exception, if not found or not defined.
    int getArraySize(int key)
        throw (TableAccessException);
    // Throw exception, if not found or
    // already defined.
    void setArraySize(int key, int size)
        throw (TableAccessException);
    // Throw exception, if not found,
    // not defined or index is out of bound.
    int getArrayElementValue(int key, int index)
        throw (TableAccessException);
    // Throw exception, if not found,
    // not defined or index is out of bound.
    void setArrayElementValue(int key,
        int index, int value)
        throw (TableAccessException);

    // Returns key of new label without name,
    // if name is 0.
    int getLabelKey(const char* name)
        throw ();
    // Throw Exception, if not found or not defined.
    PolizItem* getLabelValue(int key) const
        throw (TableAccessException);
    // Throw exception, if not found or
    // already defined.
    void setLabelValue(int key,
        PolizItem* value)
        throw (TableAccessException);
};

#endif /* PARSER_TABLES_HPP_SENTRY */
