#include "ParserTables.hpp"

const char* ParserTables::keywordStrings[] = {
    "set", "array", "goto", "if",
    "else", "while", "print", "buy",
    "sell", "make", "build", "turn",
    "join"
};

const char* ParserTables::functionStrings[] = {
    "isWin", "getWinners"
};

ParserTables::ParserTables()
    : strings(),
    variables(),
    arrays(),
    labels()
{}

ScriptKeywords ParserTables::getKeywordType(const char* str) const
    throw (TableAccessException)
{
    for (unsigned int i = 0;
        i < sizeof(keywordStrings) / sizeof(const char*);
        ++i)
    {
        if (STR_EQUAL(str, keywordStrings[i]))
            return (ScriptKeywords) i;
    }

    throw TableAccessException("Unknown keyword.",
        str, __FILE__, __LINE__);
}

ScriptGameFunctions ParserTables::getFunctionType(const char* str) const
    throw (TableAccessException)
{
    for (unsigned int i = 0;
        i < sizeof(functionStrings) / sizeof(const char*);
        ++i)
    {
        if (STR_EQUAL(str, functionStrings[i]))
            return (ScriptGameFunctions) i;
    }

    throw TableAccessException("Unknown game function.",
        str, __FILE__, __LINE__);
}

bool ParserTables::functionHasNoArguments(
    ScriptGameFunctions /*functionKey*/) const
    throw ()
{
    return true;
}

int ParserTables::getStringKey(const char* name)
    throw ()
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

char* ParserTables::getStringValue(int key) const
    throw (TableAccessException)
{
    StringElem* elem = strings.get(key);

    if (elem == 0) {
        throw TableAccessException("Bad string key.",
            key, __FILE__, __LINE__);
    }

    return elem->getName();
}

int ParserTables::getVariableKey(const char* name, bool def)
    throw (TableAccessException)
{
    int key = variables.search(name);

    if (def && key == -1) {
        throw TableAccessException("Variable not found.",
            key, __FILE__, __LINE__);
    } else if (!def && key == -1) {
        int nameSize = strlen(name) + 1;
        char* nameCopy = new char[nameSize];
        memcpy(nameCopy, name, nameSize); // with '\0'
        key = variables.add(new VariableElem(nameCopy));
    }

    return key;
}

int ParserTables::getVariableValue(int key) const
    throw (TableAccessException)
{
    VariableElem* elem =
        static_cast<VariableElem*>(variables.get(key));

    if (elem == 0) {
        throw TableAccessException("Bad variable key.",
            key, __FILE__, __LINE__);
    }

    if (! elem->isDefined()) {
        throw TableAccessException("Try to get value of"
            " undefined variable.",
            key, __FILE__, __LINE__);
    }

    return elem->getValue();
}

void ParserTables::setVariableValue(int key, int value)
    throw (TableAccessException)
{
    VariableElem* elem =
        static_cast<VariableElem*>(variables.get(key));

    if (elem == 0) {
        throw TableAccessException("Bad variable key.",
            key, __FILE__, __LINE__);
    }

    elem->setValue(value);
}

int ParserTables::getArrayKey(const char* name, bool def)
    throw (TableAccessException)
{
    int key = arrays.search(name);

    if (def && key == -1) {
        throw TableAccessException("Array not found.",
            key, __FILE__, __LINE__);
    } else if (!def && key == -1) {
        int nameSize = strlen(name) + 1;
        char* nameCopy = new char[nameSize];
        memcpy(nameCopy, name, nameSize); // with '\0'
        key = arrays.add(new ArrayElem(nameCopy));
    } else if (!def && key > -1) {
        throw TableAccessException("Double array definition.",
            key, __FILE__, __LINE__);
    }

    return key;
}

int ParserTables::getArraySize(int key)
    throw (TableAccessException)
{
    ArrayElem* elem =
        static_cast<ArrayElem*>(arrays.get(key));

    if (elem == 0) {
        throw TableAccessException("Bad array key.",
            key, __FILE__, __LINE__);
    }

    if (! elem->isDefined()) {
        throw TableAccessException("Try to get size of"
            " undefined array.",
            key, __FILE__, __LINE__);
    }

    return elem->getSize();
}

void ParserTables::setArraySize(int key, int size)
    throw (TableAccessException)
{
    ArrayElem* elem =
        static_cast<ArrayElem*>(arrays.get(key));

    if (elem == 0) {
        throw TableAccessException("Bad array key.",
            key, __FILE__, __LINE__);
    }

    if (elem->isDefined()) {
        throw TableAccessException("Try to redefine"
            " (change size) array.",
            key, __FILE__, __LINE__);
    }

    elem->setSize(size);
}

int ParserTables::getArrayElementValue(int key, int index)
    throw (TableAccessException)
{
    ArrayElem* elem =
        static_cast<ArrayElem*>(arrays.get(key));

    if (elem == 0) {
        throw TableAccessException("Bad array key.",
            key, __FILE__, __LINE__);
    }

    if (! elem->isDefined()) {
        throw TableAccessException("Try to get value"
            " of element from undefined array.",
            key, __FILE__, __LINE__);
    }

    try {
        return elem->getElementValue(index);
    } catch(ArrayAccessException& ex) {
        throw TableAccessException(ex, key,
            __FILE__, __LINE__);
    }

    return 0; // unreachable code
}

void ParserTables::setArrayElementValue(int key, int index, int value)
    throw (TableAccessException)
{
    ArrayElem* elem =
        static_cast<ArrayElem*>(arrays.get(key));

    if (elem == 0) {
        throw TableAccessException("Bad array key.",
            key, __FILE__, __LINE__);
    }

    if (! elem->isDefined()) {
        throw TableAccessException("Try to set value"
            " of element from undefined array.",
            key, __FILE__, __LINE__);
    }

    try {
        elem->setElementValue(index, value);
    } catch(ArrayAccessException& ex) {
        throw TableAccessException(ex, key,
            __FILE__, __LINE__);
    }
}

int ParserTables::getLabelKey(const char* name)
    throw ()
{
    if (name == 0) {
        return labels.add(new LabelElem(0));
    }

    int key = labels.search(name);

    if (key == -1) {
        int nameSize = strlen(name) + 1;
        char* nameCopy = new char[nameSize];
        memcpy(nameCopy, name, nameSize); // with '\0'
        key = labels.add(new LabelElem(nameCopy));
    }

    return key;
}

PolizItem* ParserTables::getLabelValue(int key) const
    throw (TableAccessException)
{
    LabelElem* elem =
        static_cast<LabelElem*>(labels.get(key));

    if (elem == 0) {
        throw TableAccessException("Bad label key.",
            key, __FILE__, __LINE__);
    }

    if (! elem->isDefined()) {
        throw TableAccessException("Try to get value of"
            " undefined label.",
            key, __FILE__, __LINE__);
    }

    return elem->getValue();
}

void ParserTables::setLabelValue(int key, PolizItem* value)
    throw (TableAccessException)
{
    LabelElem* elem =
        static_cast<LabelElem*>(labels.get(key));

    if (elem == 0) {
        throw TableAccessException("Bad label key.",
            key, __FILE__, __LINE__);
    }

    if (elem->isDefined()) {
        throw TableAccessException("Try to redefine"
            " label.", key, __FILE__, __LINE__);
    }

    elem->setValue(value);
}
