#ifndef POLIZ_ELEM_HPP_SENTRY
#define POLIZ_ELEM_HPP_SENTRY

#include <stdio.h>

class PolizElem;
#include "ParserTables.hpp"

struct PolizItem {
    PolizElem* elem;
    PolizItem* prev;
    PolizItem* next;
};

#if 0
class PolizElemIterator {
    PolizItem* cur;

public:
    PolizElemIterator(aList)
        : cur(aList)
    {}

    void prev() {
        cur = cur->prev;
    }

    void next() {
        cur = cur->next;
    }

    PolizElem* get() const
    {
        return cur->elem;
    }
};
#endif

class PolizElemStack {
    PolizItem* first;
    PolizItem* last;

public:
    PolizElemStack()
        : first(0),
        last(0)
    {}

    void push(PolizElem* elem);
    PolizElem* pop();

    PolizItem* getFirst() const;
    PolizItem* getLast() const;

    bool isEmpty() const;
};

class PolizElem {
public:
    virtual ~PolizElem() {}

    virtual void evaluate(PolizElemStack& stack,
        PolizItem*& curCmd,
        ParserTables& tables) const = 0;
};

// ==============================
// Push to stack without changes.
// ==============================

class PolizConst : public PolizElem {
    virtual PolizConst* clone() const = 0;

public:
    ~PolizConst() {}

    void evaluate(PolizElemStack& stack,
        PolizItem*& curCmd,
        ParserTables&) const;
};

template <class T>
class PolizGenericConst : public PolizConst {
    T value;

    PolizConst* clone() const
    {
        return new PolizGenericConst<T>(value);
    }

public:
    PolizGenericConst<T>(T aValue)
        : value(aValue)
    {}

    ~PolizGenericConst<T>() {}

    T get() const
    {
        return value;
    }

    static T popValue(PolizElemStack& stack)
    {
        PolizGenericConst<T>* tmp =
            dynamic_cast<PolizGenericConst<T>*>
            (stack.pop());
        if (tmp == 0) {
            throw 10; // TODO
        }
        T value = tmp->get();
        delete tmp;
        return value;
    }
};

typedef PolizGenericConst<int> PolizInt;
typedef PolizGenericConst<char*> PolizString;
typedef PolizGenericConst<PolizItem*> PolizLabel;

class PolizVariable : public PolizConst {
    int variableKey;

    PolizConst* clone() const {
        return new PolizVariable(variableKey);
    }

public:
    PolizVariable(int aVariableKey)
        : variableKey(aVariableKey)
    {}

    ~PolizVariable() {}

    int get() const
    {
        return variableKey;
    }

    static int popValue(PolizElemStack& stack)
    {
        PolizVariable* tmp =
            dynamic_cast<PolizVariable*>
            (stack.pop());
        if (tmp == 0) {
            throw 11; // TODO
        }
        int value = tmp->get();
        delete tmp;
        return value;
    }
};

// ================================
// No need access to stack pointer.
// ================================

class PolizOp : public PolizElem {
    virtual PolizElem* evaluateOp(PolizElemStack& stack,
        ParserTables& tables) const = 0;

public:
    ~PolizOp() {}

    void evaluate(PolizElemStack& stack,
        PolizItem*& curCmd,
        ParserTables& tables) const;
};

class PolizOpVariableValue : public PolizOp {
    int variableKey;

    PolizElem* evaluateOp(PolizElemStack&,
        ParserTables& tables) const;

public:
    PolizOpVariableValue(int aVariableKey)
        : variableKey(aVariableKey)
    {}
};

class PolizOpSet : public PolizOp {
    PolizElem* evaluateOp(PolizElemStack& stack,
        ParserTables& tables) const;

public:
    PolizOpSet() {}
};

class PolizOpPrint : public PolizOp {
    PolizElem* evaluateOp(PolizElemStack& stack,
        ParserTables&) const;

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

    PolizElem* evaluateOp(PolizElemStack& stack,
        ParserTables&) const;

public:
    PolizOpGame(PolizOpGameType aOp)
        : op(aOp)
    {}
};

class PolizOpInt1 : public PolizOp {
    PolizOpInt1Type op;

    PolizElem* evaluateOp(PolizElemStack& stack,
        ParserTables&) const;

public:
    PolizOpInt1(PolizOpInt1Type aOp)
        : op(aOp)
    {}
};

class PolizOpInt2 : public PolizOp {
    PolizOpInt2Type op;

    PolizElem* evaluateOp(PolizElemStack& stack,
        ParserTables&) const;

public:
    PolizOpInt2(PolizOpInt2Type aOp)
        : op(aOp)
    {}
};

// =====
// Other
// =====

class PolizGo : public PolizElem {
};

class PolizGoFalse : public PolizElem {
};

#endif /* POLIZ_ELEM_HPP_SENTRY */
