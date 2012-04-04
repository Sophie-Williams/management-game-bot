#include <stdio.h>
#include "Socket.hpp"

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: ./SocketTest host port\n");
        return 1;
    }

    try {
        Socket s(argv[1], argv[2]);
        char buffer[42];
        int readed;

        printf("%d\n", s.isConnected()); // 0

        s.connect();
        printf("%d\n", s.isConnected()); // 1

        s.write("status -a\r\n", sizeof("status -a\r\n"));

        while (s.read(buffer, sizeof(buffer), &readed)) {
            for (int i = 0; i < readed; ++i)
                putchar(buffer[i]);
        }
        putchar('\n');

        s.disconnect();
        printf("%d\n", s.isConnected()); // 0
    }
    catch(Exception& ex) {
        printf("%s", ex.toString());
    }

    return 0;
}
