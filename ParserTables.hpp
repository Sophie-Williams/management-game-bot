#ifndef PARSER_TABLES_HPP_SENTRY
#define PARSER_TABLES_HPP_SENTRY

class ParserTables;

#include "String.hpp"
#include "DynamicTable.hpp"
#include "PolizElem.hpp"
#include "TableAccessException.hpp"

#define DEFAULT_VARIABLE_VALUE 0

enum ScriptLangKeywords {
    SCR_KEYWORD_SET,
    SCR_KEYWORD_GOTO,
    SCR_KEYWORD_IF,
    SCR_KEYWORD_ELSE,
    SCR_KEYWORD_WHILE,
    SCR_KEYWORD_PRINT,
    SCR_KEYWORD_BUY,
    SCR_KEYWORD_SELL,
    SCR_KEYWORD_MAKE,
    SCR_KEYWORD_BUILD,
    SCR_KEYWORD_TURN
};

class StringConstElem : public NamedElem {
};

#if 0
template <class T>
class ValueElemGeneric : public NamedElem {
    T value;

public:
    ValueElemGeneric<T>(char* aName, int aValue)
        : NamedElem(aName),
        value(aValue)
    {}

    void setValue(T aValue)
    {
        value = aValue;
    }

    T getValue()
    {
        return value;
    }
};

// TODO: Maybe not extend, make this typedef?
//typedef NamedElem StringConstElem;
typedef ValueElemGeneric<int> VariableElem;
class PolizElem;
typedef ValueElemGeneric<PolizElem*> LabelElem;
#endif

class VariableElem : public NamedElem {
    bool defined;
    int value;

public:
    VariableElem(char* aName)
        : NamedElem(aName),
        defined(false),
        value(DEFAULT_VARIABLE_VALUE)
    {}

    bool isDefined()
    {
        return defined;
    }

    void setValue(int aValue)
    {
        defined = true;
        value = aValue;
    }

    int getValue()
    {
        return value;
    }
};

class ParserTables {
    static const char* keywordStrings[];
    DynamicTable strings;
    DynamicTable variables;
    DynamicTable labels;

public:
    ParserTables();

    ScriptLangKeywords getKeywordType(
        const char* str) const;

    // Argument def mean: "must be defined".
    // Make variable entry, if not found.
    int getVariableKey(const char* name, bool def);

    int getVariableValue(int key);

    void setVariableValue(int key, int value);
};

#endif /* PARSER_TABLES_HPP_SENTRY */
