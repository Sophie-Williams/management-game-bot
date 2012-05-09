#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Parser.hpp"
#include "GameActions.hpp"
#include "Exception.hpp"

#define OPEN_ERROR(openValue) ((openValue) == -1)
#define CLOSE_ERROR(closeValue) ((closeValue) == -1)

int main(int argc, char** argv)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s script_file host port\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);

    if (OPEN_ERROR(fd)) {
        perror("open");
        return 1;
    }

    Parser parser(fd);

    try {
        parser.parse();
        GameActions game(argv[2], argv[3]);
        parser.evaluate(game);
    } catch(Exception& ex) {
        printf("%s", ex.toString());
    }

    if (CLOSE_ERROR(close(fd))) {
        perror("close");
        return 1;
    }

    return 0;
}
