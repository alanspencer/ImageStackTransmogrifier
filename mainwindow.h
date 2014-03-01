#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDebug>

#include <QFileDialog>
#include <QMessageBox>
#include <QImageReader>
#include <QDir>

class Exception;
class Logger;
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
    Logger *log;

    int getCountDirectoryFiles(QString directory);
    QList<QString> getImageStackFileList(QString directory);
    QString getAvailableFormatsStr();

    bool isGrayScale;
    int sliceNumber;
    int imageWidth;
    int imageHeight;
    QImage::Format imageFormat;
    QString imageFormatText;
    bool inputFromOK;
    bool outputToOK;
    int processorCount;
    int chunkSize;
    QList< QList < QList<QRgb> > > chunkCacheList; // list[{z}][{x}/{y}][{y pixel data}/{x pixel data}]
    int currentTotalNumber;
    bool isRunning;
    QVector<QRgb> colorTable;
    QVector<QRgb> colorTableGray;

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
