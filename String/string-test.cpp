#include "String.hpp"
#include "IndexOutOfBoundException.hpp"

int main()
{
    String s1 = String();
    String s2 = String("abc");
    String s3 = "def";

    try {
        printf(".%d.\n", s2.isEqual(s2));      // .1.
        printf(".%d.\n", s2.isEqual("abc"));   // .1.
        printf(".%d.\n", s2.isEqual(s3));      // .0.
        printf(".%d.\n", s2.isEqual("def"));   // .0.
        printf(".%d.\n", s2.startsWith(""));            // .1.
        printf(".%d.\n", s2.startsWith("a"));           // .1.
        printf(".%d.\n", s2.startsWith("ab"));          // .1.
        printf(".%d.\n", s2.startsWith("abc"));         // .1.
        printf(".%d.\n", s2.startsWith(s2));            // .1.
        printf(".%d.\n", s2.startsWith(String("abc"))); // .1.
        printf(".%d.\n", s2.startsWith(s3));            // .0.
        printf(".%d.\n", s2.startsWith("b"));           // .0.
        s1.printf(stdout, ".%s.\n");           // ..
        s2.printf(stdout, ".%s.\n");           // .abc.
        s3.printf(stdout, ".%s.\n");           // .def.
        printf(".%c.\n", (s2+s3)[2]);          // .c.
        printf(".%c.\n", (s1+s2+s3)[2]);       // .c.
        printf(".%c.\n", (s1+s1+s3+s1+s1)[1]); // .e.
        (s2+"ghi").printf(stdout, ".%s.\n");   // .abcghi.
        s2 += s3;
        s2.printf(stdout, ".%s.\n");           // .abcdef.
        s2 += "jkl";
        s2.printf(stdout, ".%s.\n");           // .abcdefjkl.
        s2 += 'p';
        s2.printf(stdout, ".%s.\n");           // .abcdefjklp.
        printf(".%d.\n", s2.getLength());      // .10.
        s2.clear();
        printf(".%d.\n", s2.getLength());          // .0.
        (s2+'a'+'b'+'c').printf(stdout, ".%s.\n"); // .abc.
        s2[55] = 'x';                              // IndexOutOfBoundException: req 55; max: 8
    }
    catch(IndexOutOfBoundException ex) {
        ex.print(stderr);
    }

    try {
        s2[-3] = 'y';                          // IndexOutOfBoundException: req -3; max: 8
    }
    catch(IndexOutOfBoundException ex) {
        ex.print(stderr);
    }

    return 0;
}
