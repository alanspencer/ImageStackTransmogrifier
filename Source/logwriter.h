#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QList>
#include <QString>
#include <QFile>
#include <QTextStream>

#include "mainwindow.h"

class MainWindow;

class LogWriter
{
public:
    LogWriter(MainWindow *mw);

    void appendToLog(QString strTitle, QString strMessage);
    void clearLog();
    void outputToFile(QString filename);

private:
    MainWindow *mainWindow;
    QList<QString> log;
};

#endif // LOGWRITER_H
