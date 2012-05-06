#include "PolizElem.hpp"

PolizElemList::PolizElemList()
    : first(0),
    last(0)
{}

void PolizElemList::push(PolizElem* elem)
{
    PolizItem* newItem = new PolizItem;
    newItem->elem = elem;
    newItem->prev = last;
    newItem->next = 0;

    if (first == 0) {
        last = first = newItem;
    } else {
        last = last->next = newItem;
    }
}

PolizElem* PolizElemList::pop()
{
    if (first == 0) {
        throw 3; // TODO
    }

    PolizElem* res = last->elem;
    PolizItem* oldLast = last;
    last = last->prev;
    if (last == 0)
        first = 0;
    delete oldLast;

    return res;
}

PolizItem* PolizElemList::getFirst() const
{
    return first;
}

PolizItem* PolizElemList::getLast() const
{
    return last;
}

bool PolizElemList::isEmpty() const
{
    return (first == 0);
}

void PolizElemList::evaluate(PolizElemList& stack,
    ParserTables& tables) const
{
    PolizItem* cur = getFirst();

    while (cur != 0) {
        cur->elem->evaluate(stack, cur, tables);
        cur = cur->next;
    }
}

void PolizConst::evaluate(PolizElemList& stack,
    PolizItem*&, ParserTables&) const
{
    stack.push(clone());
}

void PolizOp::evaluate(PolizElemList& stack,
    PolizItem*&, ParserTables& tables) const
{
    PolizElem *res = evaluateOp(stack, tables);
    if (res != 0)
        stack.push(res);
}

PolizElem* PolizOpVariableValue::evaluateOp(PolizElemList&,
    ParserTables& tables) const
{
    return new PolizInt(
        tables.getVariableValue(variableKey));
}

PolizElem* PolizOpSet::evaluateOp(PolizElemList& stack,
    ParserTables& tables) const
{
    int value = PolizInt::popValue(stack, tables);
    int key = PolizVariable::popValue(stack, tables);

    tables.setVariableValue(key, value);
    return 0; // no push anything to stack
}

PolizElem* PolizOpPrint::evaluateOp(PolizElemList& stack,
    ParserTables& tables) const
{
    PolizElem* tmp;
    PolizInt* tmpInt;
    PolizString* tmpString;
    int args_count = PolizInt::popValue(stack, tables);
    PolizElemList argStack;

    for (int i = 0; i < args_count; ++i) {
        argStack.push(stack.pop());
    }

    for (int i = 0; i < args_count; ++i) {
        tmp = argStack.pop();
        tmpInt = dynamic_cast<PolizInt*>(tmp);
        if (tmpInt != 0) {
            printf("%d", tmpInt->getValue(tables));
            delete tmp;
            continue;
        }
        tmpString = dynamic_cast<PolizString*>(tmp);
        if (tmpString != 0) {
            printf("%s", tmpString->getValue(tables));
            delete tmp;
            continue;
        }

        throw 2; // TODO
    }

    printf("\n");

    return 0; // no push anything to stack
}


PolizElem* PolizOpGame::evaluateOp(PolizElemList& stack,
    ParserTables& tables) const
{
    int arg1;
    int arg2;

    if (op == POLIZ_OP_BUY || op == POLIZ_OP_SELL) {
        arg2 = PolizInt::popValue(stack, tables);
        arg1 = PolizInt::popValue(stack, tables);
    } else if (op == POLIZ_OP_MAKE || op == POLIZ_OP_BUILD) {
        arg1 = PolizInt::popValue(stack, tables);
    }

    // TODO: game actions
    switch (op) {
    case POLIZ_OP_BUY:
        break;
    case POLIZ_OP_SELL:
        break;
    case POLIZ_OP_MAKE:
        break;
    case POLIZ_OP_BUILD:
        break;
    case POLIZ_OP_TURN:
        break;
    }

    return 0; // no push anything to stack
}

PolizElem* PolizOpInt1::evaluateOp(PolizElemList& stack,
    ParserTables& tables) const
{
    int arg = PolizInt::popValue(stack, tables);

    switch (op) {
    case POLIZ_OP_MONADIC_PLUS:
        return new PolizInt(arg);;
    case POLIZ_OP_MONADIC_MINUS:
        return new PolizInt(-arg);;
    case POLIZ_OP_LOGIC_NOT:
        return new PolizInt(!arg);;
    }

    return 0; // it is unreachable code
}

PolizElem* PolizOpInt2::evaluateOp(PolizElemList& stack,
    ParserTables& tables) const
{
    int arg2 = PolizInt::popValue(stack, tables);
    int arg1 = PolizInt::popValue(stack, tables);

    switch (op) {
    case POLIZ_OP_MULTIPLICATION:
        return new PolizInt(arg1 * arg2);
    case POLIZ_OP_DIVISION:
        return new PolizInt(arg1 / arg2);
    case POLIZ_OP_REMAINDER_DIVISION:
        return new PolizInt(arg1 % arg2);
    case POLIZ_OP_PLUS:
        return new PolizInt(arg1 + arg2);
    case POLIZ_OP_MINUS:
        return new PolizInt(arg1 - arg2);
    case POLIZ_OP_LESS:
        return new PolizInt(arg1 < arg2);
    case POLIZ_OP_MORE:
        return new PolizInt(arg1 > arg2);
    case POLIZ_OP_EQUALITY:
        return new PolizInt(arg1 == arg2);
    case POLIZ_OP_LOGIC_AND:
        return new PolizInt(arg1 && arg2);
    case POLIZ_OP_LOGIC_OR:
        return new PolizInt(arg1 || arg2);
    }

    return 0; // it is unreachable code
}

void PolizGo::evaluate(PolizElemList& stack,
    PolizItem*& curCmd, ParserTables& tables) const
{
    curCmd = PolizLabel::popValue(stack, tables);
}
