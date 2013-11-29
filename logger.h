#ifndef LOGGER_H
#define LOGGER_H

#include <QList>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class MainWindow;

class Logger
{
public:
    Logger(MainWindow *mw);

    void append(QString strTitle, QString strMessage);
    void clear();
    void outputToFile(QString filename);

private:
    MainWindow *mainWindow;
    QList<QString> log;
};

#endif // LOGGER_H
