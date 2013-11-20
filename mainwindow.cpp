#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    processorCount = QThread::idealThreadCount();
    if (processorCount == -1) {
        processorCount = 1;
    }

    selectedDirection = X0toXn;
    reset();

    ui->supportedImageFormats->setText(getAvailableFormatsStr());

    connect(ui->inputFromButton, SIGNAL(clicked()), this, SLOT(inputFromAction()));
    connect(ui->directionX0toXn, SIGNAL(clicked()), this, SLOT(setDirectionX0toXn()));
    connect(ui->directionXntoX0, SIGNAL(clicked()), this, SLOT(setDirectionXntoX0()));
    connect(ui->directionY0toYn, SIGNAL(clicked()), this, SLOT(setDirectionY0toYn()));
    connect(ui->directionYntoY0, SIGNAL(clicked()), this, SLOT(setDirectionYntoY0()));
    connect(ui->outputToButton, SIGNAL(clicked()), this, SLOT(outputToAction()));
    connect(this, SIGNAL(dataChanged()), this, SLOT(checkRunButton()));
    connect(ui->runButton, SIGNAL(clicked()), this, SLOT(runAction()));
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(resetAction()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::reset()
{
    inputFromOK = false;
    outputToOK = false;
    sliceNumber = 0;
    imageWidth = 0;
    imageHeight = 0;
    inputFromFilename = "";
    inputFromDirectory = NULL;
    imageFormat = QImage::Format_Invalid;
    imageFormatText = "";

    ui->inputFrom->setText("");
    ui->outputTo->setText("");
    ui->numberOfSlices->setText(QString("?"));
    ui->imageFormat->setText(QString("Undefined"));
    ui->imageWidth->setText(QString("? px"));
    ui->imageHeight->setText(QString("? px"));

    for(int i = imageStackFiles.count()-1; i >= 0 ;i--)
    {
        ui->selectedStackFiles->removeRow(i);
    }
    ui->selectedStackFiles->setRowCount(0);
    imageStackFiles.clear();

    ui->inputFromButton->setEnabled(true);
    ui->directionX0toXn->setEnabled(true);
    ui->directionXntoX0->setEnabled(true);
    ui->directionY0toYn->setEnabled(true);
    ui->directionYntoY0->setEnabled(true);
    ui->outputToButton->setEnabled(true);
    ui->runButton->setEnabled(false);

    ui->totalProgressBar->reset();
    ui->totalProgressBar->setMaximum(100);

    ui->imageProgressBar->reset();
    ui->imageProgressBar->setMaximum(100);
}

void MainWindow::inputFromAction()
{
    inputFromFilename = QFileDialog::getOpenFileName(
                this,
                tr("Select First Image of Stack"),
                inputFromDirectory.absolutePath(),
                tr("Image Files (*.bmp *.tif *.tif *.jpeg *.jpg *.png)")
                );
    if (!inputFromFilename.isEmpty()) {
        inputFromDirectory = QFileInfo(inputFromFilename).absoluteDir();

        // Update GUI
        ui->inputFrom->setText(inputFromDirectory.absolutePath());       

        // Open selected image and get its width, height, type, etc...
        QImageReader image(inputFromFilename);
        if (!image.canRead()) {
            inputFromOK = false;
            QMessageBox::information(
                     this,
                     "Image Stack Transmorgifier Error",
                     QString("Cannot open %1. Only the following file formates can be opened: %2")
                        .arg(inputFromFilename)
                        .arg(getAvailableFormatsStr())
                     );
            return;
        } else {
            inputFromOK = true;

            QSize imageSize = image.size();
            imageWidth = imageSize.width();
            imageHeight = imageSize.height();

            // Work out format
            imageFormat = image.imageFormat();
            if (imageFormat == QImage::Format_Invalid) {
                inputFromOK = false;
                imageFormatText = "The images are invalid";
            } else if (imageFormat == QImage::Format_Mono) {
                inputFromOK = false;
                imageFormatText = "Monocrome MSB";
            } else if (imageFormat == QImage::Format_MonoLSB) {
                inputFromOK = false;
                imageFormatText = "Monocrome LSB";
            } else if (imageFormat == QImage::Format_Indexed8) {
                imageFormatText = "Index 8";
            } else if (imageFormat == QImage::Format_RGB32) {
                imageFormatText = "RGB 32";
            } else if (imageFormat == QImage::Format_ARGB32) {
                imageFormatText = "ARGB 32";
            } else if (imageFormat == QImage::Format_ARGB32_Premultiplied) {
                inputFromOK = false;
                imageFormatText = "ARGB 32 Premultiplied";
            } else {
                inputFromOK = false;
                imageFormatText = "Undefined";
            }

            // Update GUI
            ui->imageFormat->setText(imageFormatText);

            ui->imageWidth->setText(QString("%1 px").arg(imageWidth));
            ui->imageHeight->setText(QString("%1 px").arg(imageHeight));

            // Build list of all files
            imageStackFiles = getImageStackFileList(inputFromDirectory);
            ui->selectedStackFiles->setRowCount(imageStackFiles.count());
            for(int i = 0; i < imageStackFiles.count(); i++)
            {
                QTableWidgetItem *newItem = new QTableWidgetItem(imageStackFiles[i]);
                ui->selectedStackFiles->setItem(i, 0, newItem);
            }

            // Get number of files
            sliceNumber = getCountDirectoryFiles(inputFromDirectory);
            ui->numberOfSlices->setText(QString("%1").arg(sliceNumber));
        }
    } else {
        inputFromOK = false;
    }

    emit dataChanged();
}

void MainWindow::setDirectionX0toXn()
{
    selectedDirection = X0toXn;
}

void MainWindow::setDirectionXntoX0()
{
    selectedDirection = XntoX0;
}

void MainWindow::setDirectionY0toYn()
{
    selectedDirection = Y0toYn;
}

void MainWindow::setDirectionYntoY0()
{
    selectedDirection = YntoY0;
}

void MainWindow::outputToAction()
{
    outputToDirectory = QFileDialog::getExistingDirectory(
                this,
                tr("Output Directory"),
                "",
                QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks
                );
    if (!outputToDirectory.isEmpty()) {
        outputToOK = true;
        // Update GUI
        ui->outputTo->setText(outputToDirectory);
    } else {
        outputToOK = false;
    }

    emit dataChanged();
}


void MainWindow::runAction()
{
    ui->inputFromButton->setEnabled(false);
    ui->directionX0toXn->setEnabled(false);
    ui->directionXntoX0->setEnabled(false);
    ui->directionY0toYn->setEnabled(false);
    ui->directionYntoY0->setEnabled(false);
    ui->outputToButton->setEnabled(false);
    ui->runButton->setEnabled(false);

    int currentImageNumber = 0;

    if (selectedDirection == X0toXn) {
        ui->totalProgressBar->setMaximum(imageWidth);

        for(int x = 0; x < imageWidth; x++) // Current x value for reading
        {

            // Create new .bmp file to write to
            QImage newImage(imageHeight,imageStackFiles.count(),imageFormat);
            QVector<QRgb> colorTable;
            int imageStackFilesNum = imageStackFiles.count();
            int currentProgress = 0;

            ui->imageProgressBar->setValue(currentProgress);
            ui->imageProgressBar->setMaximum(imageStackFilesNum);

            for (int z = 0; z < imageStackFilesNum; z++) // Slice to read from
            {
                // Open Image z for reading
                QImageReader readImage(inputFromDirectory.absolutePath()+"/"+imageStackFiles[z]);
                QImage image = readImage.read();

                for(int y = 0; y < imageHeight; y++) // Pixel Color to read
                {
                    QRgb currentPixelColor = image.pixel(x, y);

                    // 8-bit Images
                    if (imageFormat == QImage::Format_Indexed8){
                        // Check if pixel colur is already in the color table
                        bool found = false;
                        for(int c = 0; c < colorTable.size(); c++)
                        {
                            if (colorTable[c] == currentPixelColor) {
                                newImage.setPixel(y, z, c);
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            int value = colorTable.size();
                            colorTable.append(currentPixelColor);
                            newImage.setColor(value, currentPixelColor);
                            newImage.setPixel(y, z, value);
                        }
                    }
                    // 32-bit Images
                    else if (imageFormat == QImage::Format_RGB32 || QImage::Format_ARGB32 || QImage::Format_ARGB32_Premultiplied) {
                        newImage.setPixel(y, z, currentPixelColor);
                    }
                }

                currentProgress++;
                ui->imageProgressBar->setValue(currentProgress);
                qApp->processEvents();
            }


            // New Filename
            QString filename = QString("%1").arg(currentImageNumber);
            if(filename.size() == 1) {
                filename.prepend("000");
            } else if (filename.size() == 2) {
                filename.prepend("00");
            } else if (filename.size() == 3) {
                filename.prepend("0");
            }
            filename.append(".bmp");

            // Save new file
            newImage.save(
                        outputToDirectory+"/"+filename,
                        "BMP"
                        );

            currentImageNumber++;
            ui->totalProgressBar->setValue(currentImageNumber);
            qApp->processEvents();
        }
    } else if (selectedDirection == XntoX0) {

    } else if (selectedDirection == Y0toYn) {

    } else if (selectedDirection == YntoY0) {

    }
}

void MainWindow::resetAction()
{
    reset();
}

void MainWindow::checkRunButton()
{
    if (outputToOK && inputFromOK) {
        ui->runButton->setEnabled(true);
    } else {
        ui->runButton->setEnabled(false);
    }
}

int MainWindow::getCountDirectoryFiles(QDir directory)
{
    directory.setFilter(QDir::Files);
    return directory.entryInfoList().size();
}


QList<QString> MainWindow::getImageStackFileList(QDir directory)
{
    QList<QString> foundFileList;
    directory.setFilter(QDir::Files);
    for(int i = 0; i < directory.entryInfoList().size(); i++)
    {
        foundFileList.append(directory.entryInfoList()[i].fileName());
    }
    return foundFileList;
}

QString MainWindow::getAvailableFormatsStr()
{
    QString availableFormats;
    QList<QByteArray> availableFormatsList = QImageReader::supportedImageFormats();
    for(int i = 0; i < availableFormatsList.count(); i++)
    {
        availableFormats += QString(availableFormatsList[i]);
        availableFormats += "; ";
    }
    return availableFormats;
}
