#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>

class Exception
{
public:
    Exception(QString str);

    QString exceptionMessage;
};

#endif // EXCEPTION_H
