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

    throw TableAccessException("unknown keyword",
        str, __FILE__, __LINE__);
}

int ParserTables::getStringKey(const char* name)
{
    int key = strings.search(name);

    if (key == -1) {
        int nameSize = strlen(name) + 1;
        char* nameCopy = new char[nameSize];
        memcpy(nameCopy, name, nameSize); // with '\0'
        key = strings.add(new StringElem(nameCopy));
    }

    return key;
}

char* ParserTables::getStringValue(int key)
{
    StringElem* elem = strings.get(key);

    if (elem == 0) {
        throw TableAccessException("bad string key",
            key, __FILE__, __LINE__);
    }

    return elem->getName();
}

int ParserTables::getVariableKey(const char* name, bool def)
{
    int key = variables.search(name);

    if (def && key == -1) {
        throw TableAccessException("bad variable key",
            key, __FILE__, __LINE__);
    } else if (!def && key == -1) {
        int nameSize = strlen(name) + 1;
        char* nameCopy = new char[nameSize];
        memcpy(nameCopy, name, nameSize); // with '\0'
        key = variables.add(new VariableElem(nameCopy));
    }

    return key;
}

int ParserTables::getVariableValue(int key)
{
    VariableElem* elem =
        static_cast<VariableElem*>(variables.get(key));

    if (elem == 0) {
        throw TableAccessException("bad variable key",
            key, __FILE__, __LINE__);
    }

    if (!elem->isDefined()) {
        throw TableAccessException("try to get value of"
            " undefined variable",
            key, __FILE__, __LINE__);
    }

    return elem->getValue();
}

void ParserTables::setVariableValue(int key, int value)
{
    VariableElem* elem =
        static_cast<VariableElem*>(variables.get(key));

    if (elem == 0) {
        throw TableAccessException("bad variable key",
            key, __FILE__, __LINE__);
    }

    elem->setValue(value);
}

int ParserTables::getLabelKey(const char* name)
{
    int key = labels.search(name);

    if (key == -1) {
        int nameSize = strlen(name) + 1;
        char* nameCopy = new char[nameSize];
        memcpy(nameCopy, name, nameSize); // with '\0'
        key = labels.add(new LabelElem(nameCopy));
    }

    return key;
}

PolizItem* ParserTables::getLabelValue(int key)
{
    LabelElem* elem =
        static_cast<LabelElem*>(labels.get(key));

    if (elem == 0) {
        throw TableAccessException("bad label key",
            key, __FILE__, __LINE__);
    }

    if (!elem->isDefined()) {
        throw TableAccessException("try to get value of"
            " undefined label",
            key, __FILE__, __LINE__);
    }

    return elem->getValue();
}

void ParserTables::setLabelValue(int key, PolizItem* value)
{
    LabelElem* elem =
        static_cast<LabelElem*>(labels.get(key));

    if (elem == 0) {
        throw TableAccessException("bad label key",
            key, __FILE__, __LINE__);
    }

    if (elem->isDefined()) {
        throw TableAccessException("try to redefine"
            " label", key, __FILE__, __LINE__);
    }

    elem->setValue(value);
}
