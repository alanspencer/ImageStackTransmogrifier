#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDebug>

#include <QFileDialog>
#include <QMessageBox>
#include <QImageReader>
#include <QDir>

#include "exception.h"
#include "logwriter.h"
#include "transmogrifier.h"

class Exception;
class LogWriter;
class Transmogrifier;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void reset();

    void setupChunkProgressBar(int maxValue);
    void setupSliceProgressBar(int maxValue);
    void setupOverallProgressBar(int maxValue);
    void setChunkProgress(int value);
    void setSliceProgress(int value);
    void setOverallProgress(int value);

    void logAppend(QString message);
    void logClear();

private:
    Ui::MainWindow *ui;
    Transmogrifier *transmogrifier;
    LogWriter *logWriter;

    int getCountDirectoryFiles(QDir directory);
    void getImageStackFileList(QDir directory);
    QString getAvailableFormatsStr();
    void transmogrifierLoadOneCopyRow();
    void transmogrifierLoadChunkCopyRows();
    void xLoadChunk(int xChunkStart, int xChunkEnd);
    void runX0toXnLoop(int xChunkStart, int xChunkEnd);
    bool isCacheEnabled();

    bool isGrayScale;
    QString inputFromFilename;
    QDir inputFromDirectory;
    Direction selectedDirection;
    QString outputToDirectory;
    int sliceNumber;
    int imageWidth;
    int imageHeight;
    QImage::Format imageFormat;
    QString imageFormatText;
    QList<QString> imageStackFiles;
    bool inputFromOK;
    bool outputToOK;
    int processorCount;
    int chunkSize;
    QList< QList < QList<QRgb> > > chunkCacheList; // list[{z}][{x}/{y}][{y pixel data}/{x pixel data}]
    int currentTotalNumber;
    bool isRunning;
    QVector<QRgb> colorTable;
    QVector<QRgb> colorTableGray;
    OutputFormat outputFormat;

private slots:
    void inputFromAction();
    void setDirectionX0toXn();
    void setDirectionXntoX0();
    void setDirectionY0toYn();
    void setDirectionYntoY0();
    void outputToAction();
    void runAction();
    void abortAction();
    void resetAction();
    void aboutAction();
    void checkRunButton();
    void setOutputFormat(int index);
    void saveLogAction();
    void clearLogAction();

signals:
    void dataChanged();
};

#endif // MAINWINDOW_H
