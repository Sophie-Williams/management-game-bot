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
        throw PolizException("Cannot"
            " pop value from empty stack",
            __FILE__, __LINE__);
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
    ParserTables& tables,
    GameActions& game) const
{
    PolizItem* cur = getFirst();

    while (cur != 0) {
        cur->elem->evaluate(stack, cur, tables, game);
        cur = cur->next;
    }
}

void PolizConst::evaluate(PolizElemList& stack,
    PolizItem*&, ParserTables&,
    GameActions&) const
{
    stack.push(clone());
}

void PolizOp::evaluate(PolizElemList& stack,
    PolizItem*&, ParserTables& tables,
    GameActions& game) const
{
    PolizElem *res = evaluateOp(stack, tables, game);
    if (res != 0) {
        stack.push(res);
    }
}

PolizElem* PolizOpVariableValue::evaluateOp(
    PolizElemList& stack,
    ParserTables& tables,
    GameActions&) const
{
    int key = PolizVariable::popKey(stack);
    return new PolizInt(tables.getVariableValue(key));
}

PolizElem* PolizOpArrayElementValue::evaluateOp(
    PolizElemList& stack,
    ParserTables& tables,
    GameActions&) const
{
    int index = PolizInt::popKey(stack);
    int key = PolizArray::popKey(stack);
    return new PolizInt(
        tables.getArrayElementValue(key, index));
}

PolizElem* PolizOpSet::evaluateOp(PolizElemList& stack,
    ParserTables& tables,
    GameActions&) const
{
    int value = PolizInt::popKey(stack);

    PolizElem* tmp = stack.pop();
    PolizVariable* tmpVariable = dynamic_cast<PolizVariable*>(tmp);

    if (tmpVariable != 0) {
        int key = tmpVariable->getKey();
        tables.setVariableValue(key, value);
        delete tmp;
        return 0; // no push anything to stack
    }

    PolizInt* tmpInt = dynamic_cast<PolizInt*>(tmp);
    if (tmpInt != 0) {
        int index = tmpInt->getKey();
        int key = PolizArray::popKey(stack);
        tables.setArrayElementValue(key, index, value);
        delete tmp;
        return 0; // no push anything to stack
    }

    throw PolizException("Cannot"
        " pop variable or int value",
        __FILE__, __LINE__);

    return 0; // no push anything to stack
}

PolizElem* PolizOpArrayDefine::evaluateOp(PolizElemList& stack,
    ParserTables& tables,
    GameActions&) const
{
    int size = PolizInt::popKey(stack);
    int key = PolizArray::popKey(stack);

    tables.setArraySize(key, size);
    return 0; // no push anything to stack
}

PolizElem* PolizOpPrint::evaluateOp(PolizElemList& stack,
    ParserTables& tables,
    GameActions&) const
{
    PolizElem* tmp;
    PolizInt* tmpInt;
    PolizString* tmpString;
    int args_count = PolizInt::popKey(stack);
    PolizElemList argStack;

    for (int i = 0; i < args_count; ++i) {
        argStack.push(stack.pop());
    }

    for (int i = 0; i < args_count; ++i) {
        tmp = argStack.pop();
        tmpInt = dynamic_cast<PolizInt*>(tmp);
        if (tmpInt != 0) {
            printf("%d", tmpInt->getKey());
            delete tmp;
            continue;
        }
        tmpString = dynamic_cast<PolizString*>(tmp);
        if (tmpString != 0) {
            int key = tmpString->getKey();
            printf("%s", tables.getStringValue(key));
            delete tmp;
            continue;
        }

        throw PolizException("Cannot"
            " pop int or string value",
            __FILE__, __LINE__);
    }

    printf("\n");

    return 0; // no push anything to stack
}


PolizElem* PolizOpGame::evaluateOp(PolizElemList& stack,
    ParserTables&,
    GameActions& game) const
{
    int arg1;
    int arg2;

    if (op == POLIZ_OP_BUY || op == POLIZ_OP_SELL) {
        arg2 = PolizInt::popKey(stack);
        arg1 = PolizInt::popKey(stack);
    } else if (op == POLIZ_OP_MAKE || op == POLIZ_OP_BUILD) {
        arg1 = PolizInt::popKey(stack);
        /* arg2 is undefined. */
    }

    switch (op) {
    case POLIZ_OP_BUY:
        game.buy(arg1, arg2);
        break;
    case POLIZ_OP_SELL:
        game.sell(arg1, arg2);
        break;
    case POLIZ_OP_MAKE:
        game.make(arg1);
        break;
    case POLIZ_OP_BUILD:
        game.build(arg1);
        break;
    case POLIZ_OP_TURN:
        game.turn();
        break;
    case POLIZ_OP_JOIN:
        game.join();
        break;
    }

    return 0; // no push anything to stack
}

PolizElem* PolizOpGameFunc::evaluateOp(PolizElemList& stack,
    ParserTables& tables,
    GameActions& game) const
{
    int arg;

    if (! tables.functionHasNoArguments(op)) {
        arg = PolizInt::popKey(stack);
    }

    switch (op) {
    case SCR_FUNC_IS_WIN:
        return new PolizInt(game.isWin());
    case SCR_FUNC_GET_WINNERS:
        int key = tables.getStringKey(game.getWinners());
        return new PolizString(key);
    }

    return 0; // no push anything to stack
}

PolizElem* PolizOpInt1::evaluateOp(PolizElemList& stack,
    ParserTables&,
    GameActions&) const
{
    int arg = PolizInt::popKey(stack);

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
    ParserTables&,
    GameActions&) const
{
    int arg2 = PolizInt::popKey(stack);
    int arg1 = PolizInt::popKey(stack);

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
    PolizItem*& curCmd, ParserTables& tables,
    GameActions&) const
{
    int key = PolizLabel::popKey(stack);
    curCmd = tables.getLabelValue(key);
}

void PolizGoFalse::evaluate(PolizElemList& stack,
    PolizItem*& curCmd, ParserTables& tables,
    GameActions&) const
{
    int key = PolizLabel::popKey(stack);
    int condition = PolizInt::popKey(stack);

    if (! condition) {
        curCmd = tables.getLabelValue(key);
    }
}
