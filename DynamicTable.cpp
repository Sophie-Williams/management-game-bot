#include "DynamicTable.hpp"

DynamicTable::DynamicTable()
{
    p = new NamedElem*[INIT_TABLE_SIZE];
    size = INIT_TABLE_SIZE;
    freeElemIndex = 0;
}

DynamicTable::~DynamicTable()
{
    for (int i = 0; i < freeElemIndex; ++i) {
        delete p[i];
    }

    delete[] p;
}

// returns key
int DynamicTable::add(NamedElem* elem)
{
    int index = freeElemIndex;

    if (index == size) {
        size += STEP_TABLE_SIZE;
        NamedElem** newPtr = new NamedElem*[size];
        delete[] p;
        p = newPtr;
    }

    p[index] = elem;
    ++freeElemIndex;
    return index;
}

NamedElem* DynamicTable::get(int key)
{
    if (key < freeElemIndex) {
        return p[key];
    }

    return 0;
}

int DynamicTable::search(const char* name)
{
    for (int i = 0; i < freeElemIndex; ++i) {
        if (p[i]->hasName(name)) {
            return i;
        }
    }

    return -1;
}
