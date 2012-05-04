#include "PolizElem.hpp"

// TODO: remove
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

PolizElemList::PolizElemList()
    : first(0),
    last(0)
{}

#if 0
PolizElemList::PolizElemList(PolizElemList& list)
    : PolizElemList()
{
    PolizElem* cur = list.getFirst();

    while (cur != 0) {
        push(cur);
        cur = cur->next;
    }
}
#endif

void PolizElemList::push(PolizElem* elem)
{
    // TODO: remove
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

PolizElem* PolizElemList::pop()
{
fprintf(stderr, "pop\n"); // TODO: remove
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

#if 0
void PolizElemList::push(PolizElemList* list)
{
    PolizElem* cur = list.getFirst();

    if (cur == 0)
        return;

    if (first == 0) {
        last = first = cur;
        cur = cur->next;
    }

    while (cur != 0) {
        last = last->next = cur;
        cur = cur->next;
    }

    delete list;
}
#endif

void PolizElemList::evaluate(PolizElemList& stack,
    ParserTables& tables) const
{
    PolizItem* cur = getFirst();

    fprintf(stderr, "evaluate {\n"); // TODO: remove

    while (cur != 0) {
        cur->elem->evaluate(stack, cur, tables);
        cur = cur->next;
    }

    fprintf(stderr, "}\n"); // TODO: remove
}

#if 0
PolizElemIterator& PolizElemList::getIterator()
{
    return new PolizElemIterator(first);
}
#endif

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
    int value = PolizInt::popValue(stack);
    int key = PolizVariable::popValue(stack);

    tables.setVariableValue(key, value);
    return 0; // no push anything to stack
}

PolizElem* PolizOpPrint::evaluateOp(PolizElemList& stack,
    ParserTables&) const
{
    PolizElem* tmp;
    PolizInt* tmpInt;
    PolizString* tmpString;
    int args_count = PolizInt::popValue(stack);
    PolizElemList argStack;

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


PolizElem* PolizOpGame::evaluateOp(PolizElemList& stack,
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

PolizElem* PolizOpInt1::evaluateOp(PolizElemList& stack,
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

PolizElem* PolizOpInt2::evaluateOp(PolizElemList& stack,
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
