#include "logwriter.h"

LogWriter::LogWriter()
{
}

void LogWriter::appendToLog(QString message)
{
    log.append(message);
}

void LogWriter::clearLog()
{
    log.clear();
}

void LogWriter::outputToScreen()
{

}

void LogWriter::outputToFile(QString fileToMake)
{

}
