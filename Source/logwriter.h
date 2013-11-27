#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QList>
#include <QString>

class LogWriter
{
public:
    LogWriter();

    void appendToLog(QString message);
    void clearLog();
    void outputToScreen();
    void outputToFile(QString fileToMake);

private:
    QList<QString> log;
};

#endif // LOGWRITER_H
