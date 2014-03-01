#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDebug>

#include <QFileDialog>
#include <QMessageBox>
#include <QImageReader>
#include <QDir>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

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

private:
    Ui::MainWindow *ui;

    enum Direction
    {
        X0toXn,
        XntoX0,
        Y0toYn,
        YntoY0
    };

    enum OutputFormat
    {
        BMPFormat,
        JPEGFormat,
        TIFFFormat,
        PNGFormat
    };

    int getCountDirectoryFiles(QDir directory);
    void getImageStackFileList(QDir directory);
    QString getAvailableFormatsStr();
    //void transmogrifierLoadOneCopyRow();
    void transmogrifierLoadChunkCopyRows();
    void xLoadChunk(int xChunkStart, int xChunkEnd);
    void runX0toXnLoop(int xChunkStart, int xChunkEnd);
    void yLoadChunk(int yChunkStart, int yChunkEnd);
    void runY0toYnLoop(int yChunkStart, int yChunkEnd);
    bool isCacheEnabled();
    const char *getOutputFormat();
    const char* getOutputExtension();

    bool isGrayScale;
    QString inputFromFilename;
    QDir inputFromDirectory;
    Direction selectedDirection;
    QString outputToDirectory;
    int sliceNumber;
    int imageWidth;
    int imageHeight;
    int startAtNumber;
    int endAtNumber;
    QImage::Format imageFormat;
    QString imageFormatText;
    QList<QString> imageStackFiles;
    bool inputFromOK;
    bool outputToOK;
    int processorCount;
    int chunkSize;
    QList< QList < QList<QRgb> > > chunkCacheList; // list[{z}][{x}/{y}][{y pixel data}/{x pixel data}]
    int currentTotalNumber;
    int currentOutputNumber;
    bool isRunning;
    QVector<QRgb> colorTable;
    QVector<QRgb> colorTableGray;
    OutputFormat outputFormat;

private slots:
    void inputFromAction();
    void updateStartEndNumber();
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

signals:
    void dataChanged();
};

#endif // MAINWINDOW_H
