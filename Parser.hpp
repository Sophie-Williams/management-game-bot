#ifndef PARSER_HPP_SENTRY
#define PARSER_HPP_SENTRY

#include <unistd.h>

#include "String.hpp"
#include "ScriptLexer.hpp"
#include "ParserException.hpp"
#include "ParserTables.hpp"
#include "CharQueueException.hpp"

#define READ_ERROR(readValue) ((readValue) == -1)
#define READ_EOF(readValue) ((readValue) == 0)

#ifndef PARSER_BUFFER_SIZE
#define PARSER_BUFFER_SIZE 4096
#endif

class Parser {
    int readFD;
    char buffer[PARSER_BUFFER_SIZE];
    ScriptLexer lexer;
    ScriptLexeme *currentLex;
    ParserTables tables;

    int getLine();
    int getPos();

    void getNextLex();
    bool tryLex(ScriptLexemeType type);
    bool tryLex(ScriptLexemeType type,
        int aIntValue);

    bool isLexMonadicOp();
    bool isLexArithmeticOp_1();
    bool isLexArithmeticOp_2();
    bool isLexCompareOp();
    bool isLexLogicOp_1();
    bool isLexLogicOp_2();

    void Program();
    void LabelOperator();
    void Operator();
    void SingleOperator();
    void ElseSuffix();
    void ArgsList_0();
    void ArgsList_1();
    void ArgsList_2();
    void PrintArgsList();
    void PrintArg();
    void Expr_0();
    void Expr_1();
    void Expr_2();
    void Expr_3();
    void Expr_4();
    void Expr_5();
    void Expr();
    void Variable(bool def);
    void ArraySuffix(bool def);
    void LabelPrefix();

public:
    Parser(int aReadFD);
    void parse();
};

#endif /* PARSER_HPP_SENTRY */
