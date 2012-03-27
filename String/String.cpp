#include "String.hpp"

String::String()
{
    p = new char[1];
    *p = '\0';
    length = 0;
}

String::String(const char *ap)
{
    length = strlen(ap);
    p = new char[length + 1];
    memcpy(p, ap, length + 1);
}

String::String(const String &s)
{
    length = s.getLength();
    p = new char[length + 1];
    memcpy(p, s.p, length + 1);
}

String::~String()
{
    length = 0;
    delete[] p;
}

String String::operator+(const String &s) const
{
    int newLength = length + s.length;
    char *newPtr = new char[newLength + 1];

    memcpy(newPtr, p, length); /* without '\0' */
    memcpy(newPtr + length, s.p, s.length + 1); /* with '\0' */
    String newStr(newPtr);
    delete[] newPtr;
    return newStr;
}

String String::operator+(const char *ap) const
{
    int argLength = strlen(ap);
    int newLength = length + argLength;
    char *newPtr = new char[newLength + 1];

    memcpy(newPtr, p, length); /* without '\0' */
    memcpy(newPtr + length, ap, argLength + 1); /* with '\0' */
    String newStr(newPtr);
    delete[] newPtr;
    return newStr;
}

String String::operator+(char c) const
{
    int newLength = length + 1;
    char *newPtr = new char[newLength + 1];

    memcpy(newPtr, p, length); /* without '\0' */
    newPtr[length] = c;
    newPtr[length + 1] = '\0';
    String newStr(newPtr);
    delete[] newPtr;
    return newStr;
}

String& String::operator+=(const String &s)
{
    int newLength = length + s.length;
    char *newPtr = new char[newLength + 1];

    memcpy(newPtr, p, length); /* without '\0' */
    delete[] p;
    memcpy(newPtr + length, s.p, s.length + 1); /* with '\0' */
    p = newPtr;
    length = newLength;
    return *this;
}

String& String::operator+=(const char *ap)
{
    int argLength = strlen(ap);
    int newLength = length + argLength;
    char *newPtr = new char[newLength + 1];

    memcpy(newPtr, p, length); /* without '\0' */
    delete[] p;
    memcpy(newPtr + length, ap, argLength + 1); /* with '\0' */
    p = newPtr;
    length = newLength;
    return *this;
}

String& String::operator+=(char c)
{
    int newLength = length + 1;
    char *newPtr = new char[newLength + 1];

    memcpy(newPtr, p, length); /* without '\0' */
    delete[] p;
    newPtr[length] = c;
    newPtr[length + 1] = '\0';
    p = newPtr;
    length = newLength;
    return *this;
}

char& String::operator[](int index)
    throw (IndexOutOfBoundException)
{
    if ((index < 0) || (index >= length)) {
        throw IndexOutOfBoundException(index, length - 1);
    }

    return p[index];
}

int String::getLength() const
{
    return length;
}

int String::isEqual(const char *ap) const
{
    if (ap == 0)
        return 0;

    return STR_EQUAL(p, ap);
}

int String::startsWith(const char *ap) const
{
    if (ap == 0)
        return 0;

    return STR_N_EQUAL(p, ap, strlen(ap));
}

int String::isEqual(const String &s) const
{
    return STR_EQUAL(p, s.p);
}

int String::startsWith(const String &s) const
{
    return STR_N_EQUAL(p, s.p, s.length);
}

void String::clear()
{
    delete[] p;
    p = new char[1];
    *p = '\0';
    length = 0;
}

void String::printf(FILE *stream, const char *format) const
{
    fprintf(stream, format, p);
}
