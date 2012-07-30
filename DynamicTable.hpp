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
        if (name != 0)
            delete[] name;
    }

    bool hasName(const char* aName) const
    {
        if (name == 0)
            return false;

        return STR_EQUAL(name, aName);
    }

    char* getName() const
    {
        return name;
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

    NamedElem* get(int key) const;

    // returns key
    int search(const char* name);
};

#endif /* DYNAMIC_TABLE_HPP_SENTRY */
