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

    int getCountDirectoryFiles(QDir directory);
    QList<QString> getImageStackFileList(QDir directory);
    QString getAvailableFormatsStr();
    void transmogrifierLoadOneCopyRow();

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

private slots:
    void inputFromAction();
    void setDirectionX0toXn();
    void setDirectionXntoX0();
    void setDirectionY0toYn();
    void setDirectionYntoY0();
    void outputToAction();
    void runAction();
    void resetAction();
    void checkRunButton();

signals:
    void dataChanged();
};

#endif // MAINWINDOW_H
