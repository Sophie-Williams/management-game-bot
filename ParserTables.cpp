#include "ParserTables.hpp"

const char* ParserTables::keywordStrings[] = {
    "set", "goto", "if", "else",
    "while", "print", "buy", "sell",
    "make", "build", "turn", ""
};

ParserTables::ParserTables()
    : strings(),
    variables(),
    labels()
{}

ScriptLangKeywords ParserTables::getKeywordType(const char* str) const
{
    for (unsigned int i = 0;
        i < sizeof(keywordStrings) / sizeof(const char*);
        ++i)
    {
        if (STR_EQUAL(str, keywordStrings[i]))
            return (ScriptLangKeywords) i;
    }

    throw TableItemNotFoundException("unknown keyword",
        str, __FILE__, __LINE__);
}

int ParserTables::getVariableKey(const char* name, bool def)
{
    int key = variables.search(name);

    if (def && key == -1) {
        throw TableItemNotFoundException("bad variable key",
            key, __FILE__, __LINE__);
    } else if (!def && key == -1) {
        int nameSize = strlen(name) + 1;
        char* nameCopy = new char[nameSize];
        memcpy(nameCopy, name, nameSize); // with '\0'
        key = variables.add(new VariableElem(nameCopy,
            DEFAULT_VARIABLE_VALUE));
    }

    return key;
}

int ParserTables::getVariableValue(int key)
{
    VariableElem* elem =
        static_cast<VariableElem*>(variables.get(key));

    if (elem == 0) {
        throw TableItemNotFoundException("bad variable key",
            key, __FILE__, __LINE__);
    }

    return elem->getValue();
}

void ParserTables::setVariableValue(int key, int value)
{
    VariableElem* elem =
        static_cast<VariableElem*>(variables.get(key));

    if (elem == 0) {
        throw TableItemNotFoundException("bad variable key",
            key, __FILE__, __LINE__);
    }

    elem->setValue(value);
}
