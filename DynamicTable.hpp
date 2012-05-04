#ifndef DYNAMIC_TABLE_HPP_SENTRY
#define DYNAMIC_TABLE_HPP_SENTRY

#define INIT_TABLE_SIZE 32
#define STEP_TABLE_SIZE 32

#include "String.hpp"

class NamedElem {
// TODO: copy string or delete in destructor?
    char* name;

public:
    NamedElem(char* aName)
        : name(aName)
    {}

    virtual ~NamedElem()
    {
        delete name;
    }

    bool hasName(const char* aName)
    {
        return STR_EQUAL(name, aName);
    }
};

class DynamicTable {
    NamedElem** p;
    int size;
    int freeElemIndex;

public:
    DynamicTable();
    ~DynamicTable();

    // returns key
    int add(NamedElem* elem);

    NamedElem* get(int key);

    // returns key
    int search(const char* name);
};

#endif /* DYNAMIC_TABLE_HPP_SENTRY */
