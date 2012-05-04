#include "PolizElem.hpp"

void printElem(PolizElem* elem)
{
PolizInt* tmp1 = dynamic_cast<PolizInt*>(elem);
if (tmp1 != 0)
    fprintf(stderr, " PolizInt: %d", tmp1->get());
PolizOpPrint* tmp2 = dynamic_cast<PolizOpPrint*>(elem);
if (tmp2 != 0)
    fprintf(stderr, " PolizOpPrint");
PolizOpVariableValue* tmp3 = dynamic_cast<PolizOpVariableValue*>(elem);
if (tmp3 != 0)
    fprintf(stderr, " PolizOpVariableValue");
PolizOpSet* tmp4 = dynamic_cast<PolizOpSet*>(elem);
if (tmp4 != 0)
    fprintf(stderr, " PolizOpSet");
PolizVariable* tmp5 = dynamic_cast<PolizVariable*>(elem);
if (tmp5 != 0)
    fprintf(stderr, " PolizVariable");
}

void PolizElemStack::push(PolizElem* elem)
{
fprintf(stderr, "push;");
printElem(elem);
fprintf(stderr, "\n");

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

PolizElem* PolizElemStack::pop()
{
fprintf(stderr, "pop\n");
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

PolizItem* PolizElemStack::getFirst() const
{
    return first;
}

PolizItem* PolizElemStack::getLast() const
{
    return last;
}

bool PolizElemStack::isEmpty() const
{
    return (first == 0);
}

#if 0
PolizElemIterator& PolizElemStack::getIterator()
{
    return new PolizElemIterator(first);
}
#endif

void PolizConst::evaluate(PolizElemStack& stack,
    PolizItem*& curCmd,
    ParserTables&) const
{
    stack.push(clone());
    curCmd = curCmd->next;
}

void PolizOp::evaluate(PolizElemStack& stack,
    PolizItem*& curCmd,
    ParserTables& tables) const
{
    PolizElem *res = evaluateOp(stack, tables);
    if (res != 0)
        stack.push(res);
    curCmd = curCmd->next;
}

PolizElem* PolizOpVariableValue::evaluateOp(PolizElemStack&,
    ParserTables& tables) const
{
    return new PolizInt(
        tables.getVariableValue(variableKey));
}

PolizElem* PolizOpSet::evaluateOp(PolizElemStack& stack,
    ParserTables& tables) const
{
    int value = PolizInt::popValue(stack);
    int key = PolizVariable::popValue(stack);

    tables.setVariableValue(key, value);
    return 0; // no push anything to stack
}

// TODO: printing from begin
PolizElem* PolizOpPrint::evaluateOp(PolizElemStack& stack,
    ParserTables&) const
{
    PolizElem* tmp;
    PolizInt* tmpInt;
    PolizString* tmpString;
    int args_count = PolizInt::popValue(stack);
    PolizElemStack argStack;

    for (int i = 0; i < args_count; ++i) {
        argStack.push(stack.pop());
    }

    for (int i = 0; i < args_count; ++i) {
        tmp = argStack.pop();
        tmpInt = dynamic_cast<PolizInt*>(tmp);
        if (tmpInt != 0) {
            printf("%d", tmpInt->get());
            delete tmp;
            continue;
        }
        tmpString = dynamic_cast<PolizString*>(tmp);
        if (tmpString != 0) {
            printf("%s", tmpString->get());
            delete tmp;
            continue;
        }

        throw 2; // TODO
    }

    printf("\n");

    return 0; // no push anything to stack
}


PolizElem* PolizOpGame::evaluateOp(PolizElemStack& stack,
    ParserTables&) const
{
    int arg1;
    int arg2;

    if (op == POLIZ_OP_BUY || op == POLIZ_OP_SELL) {
        arg2 = PolizInt::popValue(stack);
        arg1 = PolizInt::popValue(stack);
    } else if (op == POLIZ_OP_MAKE || op == POLIZ_OP_BUILD) {
        arg1 = PolizInt::popValue(stack);
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

PolizElem* PolizOpInt1::evaluateOp(PolizElemStack& stack,
    ParserTables&) const
{
    int arg = PolizInt::popValue(stack);

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

PolizElem* PolizOpInt2::evaluateOp(PolizElemStack& stack,
    ParserTables&) const
{
    int arg2 = PolizInt::popValue(stack);
    int arg1 = PolizInt::popValue(stack);

    switch (op) {
    case POLIZ_OP_MULTIPLICATION:
        return new PolizInt(arg1 * arg2);;
    case POLIZ_OP_DIVISION:
        return new PolizInt(arg1 / arg2);;
    case POLIZ_OP_REMAINDER_DIVISION:
        return new PolizInt(arg1 % arg2);;
    case POLIZ_OP_PLUS:
        return new PolizInt(arg1 + arg2);;
    case POLIZ_OP_MINUS:
        return new PolizInt(arg1 - arg2);;
    case POLIZ_OP_LESS:
        return new PolizInt(arg1 < arg2);;
    case POLIZ_OP_MORE:
        return new PolizInt(arg1 > arg2);;
    case POLIZ_OP_EQUALITY:
        return new PolizInt(arg1 == arg2);;
    case POLIZ_OP_LOGIC_AND:
        return new PolizInt(arg1 && arg2);;
    case POLIZ_OP_LOGIC_OR:
        return new PolizInt(arg1 || arg2);;
    }

    return 0; // it is unreachable code
}
