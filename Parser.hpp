#ifndef PARSER_HPP_SENTRY
#define PARSER_HPP_SENTRY

#include <unistd.h>

#include "String.hpp"
#include "ScriptLexer.hpp"
#include "ParserException.hpp"
#include "ParserTables.hpp"
#include "TableAccessException.hpp"
#include "CharQueueException.hpp"
#include "PolizElem.hpp"
#include "GameActions.hpp"

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
    PolizElemList poliz;
    ParserTables tables;

    int getLine() const;
    int getPos() const;

    void getNextLex();
    bool tryLex(ScriptLexemeType type) const;
    bool tryLex(ScriptLexemeType type,
        int aIntValue) const;

    bool isLexMonadicOp() const;
    bool isLexArithmeticOp_1() const;
    bool isLexArithmeticOp_2() const;
    bool isLexCompareOp() const;
    bool isLexLogicOp_1() const;
    bool isLexLogicOp_2() const;
    bool isOperator() const;

    PolizOpInt1Type getPolizOpInt1Type(int op)
        const;
    PolizOpInt2Type getPolizOpInt2Type(int op)
        const;

    void Program();
    void LabelOperator();
    void Operator();
    void SingleOperator();
    /* Returns true, if POLIZ for
     * else branch has been constructed. */
    bool ElseSuffix(int falseLabelKey);
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
    /* Argument def mean: "must be defined".
     * See README for more information. */
    void Variable(bool def);
    /* Returns true, if POLIZ for array
     * has been constructed. */
    bool ArraySuffix(const char* name,
        bool def);
    void ArrayDefine();
    void LabelPrefix();

public:
    Parser(int aReadFD);
    void parse();
    void evaluate(GameActions& game);
};

#endif /* PARSER_HPP_SENTRY */
