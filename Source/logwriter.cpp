#include "logwriter.h"

LogWriter::LogWriter(MainWindow *mw)
{
    mainWindow = mw;
}

void LogWriter::append(QString strTitle, QString strMessage)
{
    QString logText;
    QString logHTML;

    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();

    logText = QString("%1: %2 - %3")
            .arg(dateTimeString)
            .arg(strTitle)
            .arg(strMessage);
    log.append(logText);

    logHTML = QString("<div><span>%1</span> - <span><b>%2:</b> <i>%3</i></span></div>")
            .arg(dateTimeString)
            .arg(strTitle)
            .arg(strMessage);
    mainWindow->logAppend(logHTML);
}

void LogWriter::clear()
{
    log.clear();
    mainWindow->logClear();
}

void LogWriter::outputToFile(QString filename)
{

    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    for(int i = 0; i < log.count(); i++)
    {
        out << log[i];
    }

    file.close();
}
