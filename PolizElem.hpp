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
    // key is key in ParserTables or
    // value for PolizInt.
    int key;

    virtual PolizConst* clone() const = 0;

protected:
    static int elemToKey(PolizConst* elem)
    {
        if (elem == 0) {
            throw PolizException("Cannot"
                " pop value of requested type",
                __FILE__, __LINE__);
        }

        int key = elem->getKey();
        delete elem;
        return key;
    }

public:
    PolizConst(int aKey)
        : key(aKey)
    {}

    int getKey() const
    {
        return key;
    }

    void evaluate(PolizElemList& stack,
        PolizItem*&, ParserTables&) const;
};

#if 0
template <class T>
class PolizGenericConst : public PolizConst {
public:
    PolizGenericConst<T>(int aKey)
        : PolizConst(aKey)
    {}

    virtual int getKey() const = 0;

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
#endif

class PolizInt : public PolizConst {
    PolizConst* clone() const
    {
        return new PolizInt(getKey());
    }

public:
    PolizInt(int aKey)
        : PolizConst(aKey)
    {}

    static int popKey(PolizElemList& stack)
    {
        return elemToKey(
            dynamic_cast<PolizInt*>(stack.pop()));
    }
};

class PolizString : public PolizConst {
    PolizConst* clone() const
    {
        return new PolizString(getKey());
    }

public:
    PolizString(int aKey)
        : PolizConst(aKey)
    {}

    static int popKey(PolizElemList& stack)
    {
        return elemToKey(
            dynamic_cast<PolizString*>(stack.pop()));
    }
};

class PolizVariable : public PolizConst {
    PolizConst* clone() const
    {
        return new PolizVariable(getKey());
    }

public:
    PolizVariable(int aKey)
        : PolizConst(aKey)
    {}

    static int popKey(PolizElemList& stack)
    {
        return elemToKey(
            dynamic_cast<PolizVariable*>(stack.pop()));
    }
};

class PolizArray : public PolizConst {
    PolizConst* clone() const
    {
        return new PolizArray(getKey());
    }

public:
    PolizArray(int aKey)
        : PolizConst(aKey)
    {}

    static int popKey(PolizElemList& stack)
    {
        return elemToKey(
            dynamic_cast<PolizArray*>(stack.pop()));
    }
};

class PolizLabel : public PolizConst {
    PolizConst* clone() const
    {
        return new PolizLabel(getKey());
    }

public:
    PolizLabel(int aKey)
        : PolizConst(aKey)
    {}

    static int popKey(PolizElemList& stack)
    {
        return elemToKey(
            dynamic_cast<PolizLabel*>(stack.pop()));
    }
};

// ================================
// No need access to stack pointer.
// ================================

class PolizOp : public PolizElem {
    virtual PolizElem* evaluateOp(PolizElemList& stack,
        ParserTables& tables) const = 0;

public:
    void evaluate(PolizElemList& stack,
        PolizItem*&, ParserTables&) const;
};

class PolizOpVariableValue : public PolizOp {
    PolizElem* evaluateOp(PolizElemList&,
        ParserTables& tables) const;

public:
    PolizOpVariableValue() {}
};

class PolizOpArrayElementValue : public PolizOp {
    PolizElem* evaluateOp(PolizElemList&,
        ParserTables& tables) const;

public:
    PolizOpArrayElementValue() {}
};

class PolizOpSet : public PolizOp {
    PolizElem* evaluateOp(PolizElemList& stack,
        ParserTables& tables) const;

public:
    PolizOpSet() {}
};

class PolizOpArrayDefine : public PolizOp {
    PolizElem* evaluateOp(PolizElemList&,
        ParserTables& tables) const;

public:
    PolizOpArrayDefine() {}
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
        ParserTables&) const;

public:
    PolizOpGame(PolizOpGameType aOp)
        : op(aOp)
    {}
};

class PolizOpInt1 : public PolizOp {
    PolizOpInt1Type op;

    PolizElem* evaluateOp(PolizElemList& stack,
        ParserTables&) const;

public:
    PolizOpInt1(PolizOpInt1Type aOp)
        : op(aOp)
    {}
};

class PolizOpInt2 : public PolizOp {
    PolizOpInt2Type op;

    PolizElem* evaluateOp(PolizElemList& stack,
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
