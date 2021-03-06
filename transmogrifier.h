#ifndef TRANSMOGRIFIER_H
#define TRANSMOGRIFIER_H

#include <QWidget>

class Logger;
class MainWindow;

class Transmogrifier : public QWidget
{
public:
    Transmogrifier(MainWindow *mw, Logger *l);
    void reset();

    MainWindow *mainWindow;
    Logger *log;

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
    bool setInputDirectory(QString inDir);
    bool setOutputDirectory(QString outDir);
    void setXValues(int value, int start = 0, int end = 0);
    void setYValues(int value, int start = 0, int end = 0);
    void setZValues(int value, int start = 0, int end = 0);
    void setChunkSize(int size);
    void setIsGrayscale(bool grayscale);

    Transmogrifier::Direction getDirection();
    Transmogrifier::CodeVersion getCodeVersion();
    Transmogrifier::OutputFormat getOutputFormat();
    QString getOutputFormatText();
    QString getOutputExtension();
    QImage::Format getImageFormat();
    bool getRunningStatus();
    bool getIsGrayscale();
    QString getImageFormatText();
    bool getImageFormatIsValid();

    void run();
    void abort();

    void createGrayscaleColorTable();

private:
    Ui::transmogrifierForm *form;

    void setupProgressBars();
    void updateChuckProgress(int value);
    void updateSliceProgress(int value);
    void updateOverallProgress(int value);
    void populateInputFileList();
    int getInputFilesCount();

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
    QList<QString> inputDirectoryFiles;
    QString outputToDirectory;
    QList< QList < QList<QRgb> > > chunkCacheList; // list[{z}][{chunk number}][{y pixel data}/{x pixel data}]
    QVector<QRgb> colorTable;
    QVector<QRgb> colorTableGray;
};

#endif // TRANSMOGRIFIER_H
