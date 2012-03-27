#ifndef STRING_HPP_SENTRY
#define STRING_HPP_SENTRY

/* TODO из заметок:
Подумать над реализацией принятия в себя
трёх видов (char *) (как в интерпретаторе tcl):
1. const — будут жить всегда.
2. Неконст., но пусть free/delete делает String.
3. Неконст., надо копировать. */

#include <string.h>
#include <stdio.h>
#include "IndexOutOfBoundException.hpp"

/* Case insensitive. */
#define STR_EQUAL_CASE_INS(str1, str2) \
    (strcasecmp((str1), (str2)) == 0)
/* Case sensitive. */
#define STR_EQUAL(str1, str2) \
    (strcmp((str1), (str2)) == 0)
#define STR_N_EQUAL(str1, str2, length) \
    (strncmp((str1), (str2), length) == 0)

class String {
#if 0
    static const char emptyStringPtr[] = "";
#endif
    char *p; /* null-terminated */
    int length; /* without '\0' */
public:
    String();
    String(const char *ap);
    String(const String &s);
    ~String();
    String operator+(const String &s) const;
    String operator+(const char *ap) const;
    String operator+(char c) const;
    String& operator+=(const String &s);
    String& operator+=(const char *ap);
    String& operator+=(char c);
    char& operator[](int index)
        throw (IndexOutOfBoundException);
    int getLength() const;
    int isEqual(const char *ap) const;
    int startsWith(const char *ap) const;
    int isEqual(const String &s) const;
    int startsWith(const String &s) const;
    void clear();
    void printf(FILE *stream, const char *format) const;
};

#endif // STRING_HPP_SENTRY
