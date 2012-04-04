#ifndef EXCEPTION_HPP_SENTRY
#define EXCEPTION_HPP_SENTRY

class Exception {
    const char* file;
    int line;

protected:
    const char* getFile() const
    {
        return file;
    }

    int getLine() const
    {
        return line;
    }

public:
    Exception(const char* aFile, int aLine)
        : file(aFile), line(aLine) {}

    virtual ~Exception() {}

    virtual const char* toString() const = 0;
};

#endif /* EXCEPTION_HPP_SENTRY */
