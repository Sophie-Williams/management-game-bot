#ifndef POLIZ_ELEM_HPP_SENTRY
#define POLIZ_ELEM_HPP_SENTRY

#include <stdio.h>

class PolizElem;

#include "ParserTables.hpp"
#include "PolizException.hpp"

struct PolizItem {
    PolizElem* elem;
    PolizItem* prev;
    PolizItem* next;
};

class PolizElemList {
    PolizItem* first;
    PolizItem* last;

public:
    PolizElemList();
    ~PolizElemList() {}

    void push(PolizElem* elem);
    PolizElem* pop();

    PolizItem* getFirst() const;
    PolizItem* getLast() const;

    bool isEmpty() const;

    void evaluate(PolizElemList& stack,
        ParserTables& tables) const;
};

class PolizElem {
public:
    virtual ~PolizElem() {}

    virtual void evaluate(PolizElemList& stack,
        PolizItem*& curCmd,
        ParserTables& tables) const = 0;
};

// ==============================
// Push to stack without changes.
// ==============================

class PolizConst : public PolizElem {
    int key;

    virtual PolizConst* clone() const = 0;

protected:
    int getKey() const
    {
        return key;
    }

public:
    PolizConst(int aKey)
        : key(aKey)
    {}

    ~PolizConst() {}

    void evaluate(PolizElemList& stack,
        PolizItem*&, ParserTables&) const;
};

template <class T>
class PolizGenericConst : public PolizConst {
public:
    PolizGenericConst<T>(int aKey)
        : PolizConst(aKey)
    {}

    ~PolizGenericConst<T>() {}

    virtual T getValue(ParserTables& tables)
        const = 0;

    static T popValue(PolizElemList& stack,
        ParserTables& tables)
    {
        PolizGenericConst<T>* tmp =
            dynamic_cast<PolizGenericConst<T>*>
            (stack.pop());

        if (tmp == 0) {
            throw PolizException("Cannot"
                " pop value of requested type",
                __FILE__, __LINE__);
        }

        T value = tmp->getValue(tables);
        delete tmp;
        return value;
    }
};

class PolizInt : public
    PolizGenericConst<int> {

    PolizConst* clone() const
    {
        return new PolizInt(getKey());
    }

public:
    PolizInt(int aKey)
        : PolizGenericConst<int>(aKey)
    {}

    int getValue(ParserTables&) const
    {
        return getKey();
    }
};

class PolizString : public
    PolizGenericConst<char*> {

    PolizConst* clone() const
    {
        return new PolizString(getKey());
    }

public:
    PolizString(int aKey)
        : PolizGenericConst<char*>(aKey)
    {}

    char* getValue(ParserTables& tables) const
    {
        return tables.getStringValue(getKey());
    }
};

class PolizVariable : public
    PolizGenericConst<int> {

    PolizConst* clone() const
    {
        return new PolizVariable(getKey());
    }

public:
    PolizVariable(int aKey)
        : PolizGenericConst<int>(aKey)
    {}

    int getValue(ParserTables&) const
    {
        return getKey();
    }
};

class PolizLabel : public
    PolizGenericConst<PolizItem*> {

    PolizConst* clone() const
    {
        return new PolizLabel(getKey());
    }

public:
    PolizLabel(int aKey)
        : PolizGenericConst<PolizItem*>(aKey)
    {}

    PolizItem* getValue(ParserTables& tables) const
    {
        return tables.getLabelValue(getKey());
    }
};

// ================================
// No need access to stack pointer.
// ================================

class PolizOp : public PolizElem {
    virtual PolizElem* evaluateOp(PolizElemList& stack,
        ParserTables& tables) const = 0;

public:
    ~PolizOp() {}

    void evaluate(PolizElemList& stack,
        PolizItem*&, ParserTables&) const;
};

class PolizOpVariableValue : public PolizOp {
    int variableKey;

    PolizElem* evaluateOp(PolizElemList&,
        ParserTables& tables) const;

public:
    PolizOpVariableValue(int aVariableKey)
        : variableKey(aVariableKey)
    {}
};

class PolizOpSet : public PolizOp {
    PolizElem* evaluateOp(PolizElemList& stack,
        ParserTables& tables) const;

public:
    PolizOpSet() {}
};

class PolizOpPrint : public PolizOp {
    PolizElem* evaluateOp(PolizElemList& stack,
        ParserTables& tables) const;

public:
    PolizOpPrint() {}
};

/* Game operators */
enum PolizOpGameType {
    POLIZ_OP_BUY,
    POLIZ_OP_SELL,
    POLIZ_OP_MAKE,
    POLIZ_OP_BUILD,
    POLIZ_OP_TURN
};

/* Operations with 1 argument */
enum PolizOpInt1Type {
    POLIZ_OP_MONADIC_PLUS,
    POLIZ_OP_MONADIC_MINUS,
    POLIZ_OP_LOGIC_NOT
};

/* Operations with 2 arguments */
enum PolizOpInt2Type {
    POLIZ_OP_MULTIPLICATION,
    POLIZ_OP_DIVISION,
    POLIZ_OP_REMAINDER_DIVISION,
    POLIZ_OP_PLUS,
    POLIZ_OP_MINUS,
    POLIZ_OP_LESS,
    POLIZ_OP_MORE,
    POLIZ_OP_EQUALITY,
    POLIZ_OP_LOGIC_AND,
    POLIZ_OP_LOGIC_OR
};

class PolizOpGame : public PolizOp {
    PolizOpGameType op;

    PolizElem* evaluateOp(PolizElemList& stack,
        ParserTables& tables) const;

public:
    PolizOpGame(PolizOpGameType aOp)
        : op(aOp)
    {}
};

class PolizOpInt1 : public PolizOp {
    PolizOpInt1Type op;

    PolizElem* evaluateOp(PolizElemList& stack,
        ParserTables& tables) const;

public:
    PolizOpInt1(PolizOpInt1Type aOp)
        : op(aOp)
    {}
};

class PolizOpInt2 : public PolizOp {
    PolizOpInt2Type op;

    PolizElem* evaluateOp(PolizElemList& stack,
        ParserTables& tables) const;

public:
    PolizOpInt2(PolizOpInt2Type aOp)
        : op(aOp)
    {}
};

// =====
// Other
// =====

class PolizGo : public PolizElem {
public:
    void evaluate(PolizElemList& stack,
        PolizItem*& curCmd,
        ParserTables& tables) const;
};

class PolizGoFalse : public PolizElem {
public:
    void evaluate(PolizElemList& stack,
        PolizItem*& curCmd,
        ParserTables& tables) const;
};

#endif /* POLIZ_ELEM_HPP_SENTRY */
