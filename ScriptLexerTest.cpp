#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "ScriptLexer.hpp"
#include "Exception.hpp"

#define OPEN_ERROR(openValue) ((openValue) == -1)
#define READ_ERROR(readValue) ((readValue) == -1)
#define READ_EOF(readValue) ((readValue) == 0)

/* Returns:
 * 1, if lexer want to new data;
 * 0, otherwise. */
int printLexemes(ScriptLexer& lexer)
{
    ScriptLexeme *lex;

    do {
        lex = lexer.getLex();

        if (lex == 0)
            return 1;

        lexer.print(stdout, lex);

        if (lex->type == SCR_LEX_EOF ||
            lex->type == SCR_LEX_ERROR)
        {
            return 0;
        }
    } while (1);
}

int main(int argc, char **argv)
{
    ScriptLexer lexer;
    char buffer[4096];
    int readValue;
    int fd;

    if (argc != 2) {
        fprintf(stderr, "Usage: ./ScriptLexerTest script_file\n");
        return 1;
    }

    fd = open(*(argv + 1), O_RDONLY);
    if (OPEN_ERROR(fd)) {
        perror("open");
        return 1;
    }

    try {
        do {
            readValue = read(fd, &buffer, sizeof(buffer));
            if (READ_ERROR(readValue)) {
                perror("read");
                break;
            } else if (READ_EOF(readValue)) {
                fprintf(stderr, "lexer.putEOF();\n");
                lexer.putEOF();
            } else {
                fprintf(stderr, "lexer.putNewData(buffer, %d);\n", readValue);
                lexer.putNewData(buffer, readValue);
            }

            if (!printLexemes(lexer))
                break;
        } while (1);
    } catch(Exception& ex)
    {
        printf("Exception: %s\n", ex.toString());
    }

    return 0;
}
