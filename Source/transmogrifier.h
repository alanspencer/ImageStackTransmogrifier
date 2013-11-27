#ifndef TRANSMOGRIFIER_H
#define TRANSMOGRIFIER_H

#include <QString>
#include <QList>
#include <QImage>

#include "mainwindow.h"

class MainWindow;

class Transmogrifier
{
public:
    Transmogrifier(MainWindow *mw);
    void reset();

    enum Direction
    {
        X0toXn,
        XntoX0,
        Y0toYn,
        YntoY0
    };

    enum CodeVersion
    {
        StandardCode,
        ChunkedCode
    };

    enum OutputFormat
    {
        BMPFormat,
        JPEGFormat,
        TIFFFormat,
        PNGFormat
    };

    void setDirection(Direction dir);
    void setCodeVersion(CodeVersion cVer);
    void setOutputFormat(OutputFormat of);
    void setImageFormat(QImage::Format format);
    void setXValues(int value, int start = 0, int end = 0);
    void setYValues(int value, int start = 0, int end = 0);
    void setZValues(int value, int start = 0, int end = 0);
    void setFileStructure(QString inDir, QList<QString> fList, QString outDir);
    void setChunkSize(int size);
    void setIsGrayscale(bool grayscale);

    Transmogrifier::Direction getDirection();
    Transmogrifier::CodeVersion getCodeVersion();
    Transmogrifier::OutputFormat getOutputFormat();
    QImage::Format getImageFormat();
    bool getRunningStatus();
    bool getIsGrayscale();
    QString getImageFormatText();
    bool getImageFormatIsValid();

    void run();
    void abort();

    void createGrayscaleColorTable();

private:

    void setupProgressBars();
    void updateChuckProgress(int value);
    void updateSliceProgress(int value);
    void updateOverallProgress(int value);

    const char *getOutputFormat();
    const char* getOutputExtension();

    MainWindow *mainWindow;

    Direction direction;
    CodeVersion codeVersion;
    OutputFormat outputFormat;

    bool running;
    int zValue;
    int xValue;
    int yValue;
    int zStartValue;
    int zEndValue;
    int xStartValue;
    int xEndValue;
    int yStartValue;
    int yEndValue;
    int chunkSize;
    bool isGrayscale;
    QImage::Format imageFormat;
    QString inputFromDirectory;
    QList<QString> inputStackFiles;
    QString outputToDirectory;
    QList< QList < QList<QRgb> > > chunkCacheList; // list[{z}][{chunk number}][{y pixel data}/{x pixel data}]
    QVector<QRgb> colorTable;
    QVector<QRgb> colorTableGray;
};

#endif // TRANSMOGRIFIER_H
