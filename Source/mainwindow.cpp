#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    transmogrifier = new Transmogrifier(this);

    reset();

    // Create grayscale color table
    for(int c = 0; c < 255; c++)
    {
        colorTableGray.append(QColor(c,c,c).rgb());
    }


    ui->supportedImageFormats->setText(getAvailableFormatsStr());

    connect(ui->inputFromButton, SIGNAL(clicked()), this, SLOT(inputFromAction()));
    connect(ui->directionX0toXn, SIGNAL(clicked()), this, SLOT(setDirectionX0toXn()));
    connect(ui->directionXntoX0, SIGNAL(clicked()), this, SLOT(setDirectionXntoX0()));
    connect(ui->directionY0toYn, SIGNAL(clicked()), this, SLOT(setDirectionY0toYn()));
    connect(ui->directionYntoY0, SIGNAL(clicked()), this, SLOT(setDirectionYntoY0()));
    connect(ui->outputFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(setOutputFormat(int)));
    connect(ui->outputToButton, SIGNAL(clicked()), this, SLOT(outputToAction()));
    connect(this, SIGNAL(dataChanged()), this, SLOT(checkRunButton()));
    connect(ui->runButton, SIGNAL(clicked()), this, SLOT(runAction()));
    connect(ui->abortButton, SIGNAL(clicked()), this, SLOT(abortAction()));
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(resetAction()));
    connect(ui->aboutButton, SIGNAL(clicked()), this, SLOT(aboutAction()));
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
    chunkSize = 100;
    chunkCacheList.clear();
    currentTotalNumber = 0;
    isRunning = false;
    isGrayScale = false;

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

    ui->useCache->setChecked(true);
    ui->cacheChuckSize->setValue(50);

    ui->inputFromButton->setEnabled(true);
    ui->directionX0toXn->setEnabled(true);
    ui->directionXntoX0->setEnabled(true);
    ui->directionY0toYn->setEnabled(true);
    ui->directionYntoY0->setEnabled(true);
    ui->outputToButton->setEnabled(true);
    ui->runButton->setEnabled(false);
    ui->abortButton->setEnabled(false);
    ui->closeButton->setEnabled(true);
    ui->resetButton->setEnabled(true);
    ui->aboutButton->setEnabled(true);

    ui->totalProgressBar->setValue(0);
    ui->imageProgressBar->setValue(0);
    ui->chunkProgressBar->setValue(0);
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
            transmogrifier->setImageFormat(imageFormat);

            if (imageFormat == QImage::Format_Indexed8) {
                transmogrifier->setIsGrayscale(image.read().allGray());
            } else if (QImage::Format_RGB32 || QImage::Format_ARGB32 || QImage::Format_ARGB32_Premultiplied) {
                transmogrifier->setIsGrayscale(image.read().isGrayscale());
            }

            // Image Format Text
            inputFromOK = transmogrifier->getImageFormatIsValid();
            ui->imageFormat->setText(transmogrifier->getImageFormatText());

            ui->imageWidth->setText(QString("%1 px").arg(imageWidth));
            ui->imageHeight->setText(QString("%1 px").arg(imageHeight));

            // Build list of all files
            getImageStackFileList(inputFromDirectory);
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
    transmogrifier->setDirection(Transmogrifier::X0toXn);
}

void MainWindow::setDirectionXntoX0()
{
    transmogrifier->setDirection(Transmogrifier::XntoX0);
}

void MainWindow::setDirectionY0toYn()
{
    transmogrifier->setDirection(Transmogrifier::Y0toYn);
}

void MainWindow::setDirectionYntoY0()
{
    transmogrifier->setDirection(Transmogrifier::YntoY0);
}

void MainWindow::setOutputFormat(int index)
{
   QString selected =  ui->outputFormat->itemText(index);

   if (selected == "BMP") {
       transmogrifier->setOutputFormat(Transmogrifier::BMPFormat);
   } else if (selected == "JPEG") {
       transmogrifier->setOutputFormat(Transmogrifier::JPEGFormat);
   } else if (selected == "TIFF") {
       transmogrifier->setOutputFormat(Transmogrifier::TIFFFormat);
   } else if (selected == "PNG") {
       transmogrifier->setOutputFormat(Transmogrifier::PNGFormat);
   }
}

void MainWindow::outputToAction()
{
    outputToDirectory = QFileDialog::getExistingDirectory(
                this,
                tr("Output Directory"),
                "",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
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
    ui->abortButton->setEnabled(true);
    ui->closeButton->setEnabled(false);
    ui->resetButton->setEnabled(false);
    ui->aboutButton->setEnabled(false);

    // Set file structure data
    try {
        transmogrifier->setFileStructure(inputFromDirectory.absolutePath(), imageStackFiles, outputToDirectory);
        transmogrifier->setXValues(imageWidth);
        transmogrifier->setYValues(imageHeight);
        transmogrifier->setZValues(sliceNumber);

        if (isCacheEnabled()) {
            // Is chunked
            transmogrifier->setCodeVersion(Transmogrifier::ChunkedCode);
            transmogrifier->setChunkSize(ui->cacheChuckSize->value());
        }

        // Run
        transmogrifier->run();
    } catch (Exception x) {
        qDebug() << "EXCEPTION:" << x.exceptionMessage;
        //logError(x.exceptionMessage);
    }

    transmogrifier->reset();
    reset();
}

void MainWindow::transmogrifierLoadChunkCopyRows()
{
    currentTotalNumber = 0;

    if (selectedDirection == X0toXn) {
        ui->totalProgressBar->setMaximum(imageWidth);

        int xChunkStart = 0;
        int xChunkEnd = chunkSize;
        int numChunks = imageWidth/chunkSize;
        int remainderChunks = imageWidth-(numChunks*chunkSize);

        // Do chunks...
        for(int n = 0; n < numChunks; n++)  // loop #0
        {
            xLoadChunk(xChunkStart, xChunkEnd);
            runX0toXnLoop(xChunkStart, xChunkEnd);
            xChunkStart = xChunkEnd;
            xChunkEnd = xChunkEnd+chunkSize;
        }

        // Do remainder chunks...
        if (remainderChunks > 0) {
            xChunkStart = imageWidth-remainderChunks;
            xChunkEnd = imageWidth;
            xLoadChunk(xChunkStart, xChunkEnd);
            runX0toXnLoop(xChunkStart, xChunkEnd);
        }

    } else if (selectedDirection == XntoX0) {
        // TO DO... same as above but in reserve order

    } else if (selectedDirection == Y0toYn) {
        // TO DO... same as X0toXn but with x and y reversed

    } else if (selectedDirection == YntoY0) {
        // TO DO... same as above but in reserve order

    }
}

void MainWindow::xLoadChunk(int xChunkStart, int xChunkEnd)
{
    chunkCacheList.clear();

    qDebug() << "Loading Chunks " << xChunkStart << "to" << xChunkEnd << "...";

    int currentChunkProgress = 0;
    ui->chunkProgressBar->setValue(currentChunkProgress);
    ui->chunkProgressBar->setMaximum(imageStackFiles.count());

    QList< QList<QRgb> > sliceData;
    QList<QRgb> columnData;

    for (int z = 0; z < imageStackFiles.count(); z++) // loop #2
    {
        // Open Image z for reading
        QImage image = QImage(inputFromDirectory.absolutePath()+"/"+imageStackFiles[z]);
        bool breakOuterLoop = false;

        for(int x = xChunkStart;  x < xChunkEnd; x++)  // loop #1 - get rows
        {
            if (x > imageWidth) {
                breakOuterLoop = true;
                break;
            }

            // Save desired chuck           
            for(int y = 0; y < imageHeight; y++) // loop #3 - gets columns
            {
                columnData.append(image.pixel(x, y));
            }
            sliceData.append(columnData);
            columnData.clear();
        }
        chunkCacheList.append(sliceData);
        sliceData.clear();

        qDebug() << "Chunk " << xChunkStart << "to" << xChunkEnd << " for slice " << z << "loaded.";

        if (breakOuterLoop) {
            break;
        }

        currentChunkProgress++;
        ui->chunkProgressBar->setValue(currentChunkProgress);
    }
}

void MainWindow::runX0toXnLoop(int xChunkStart, int xChunkEnd)
{
    qDebug() << "Creating Slices  from Chunks " << xChunkStart << "to" << xChunkEnd << "...";

    // Current x value for reading
    int xList = 0;
    for(int x = xChunkStart; x < xChunkEnd; x++)  // loop #1
    {
        // Create new file to write to
        QImage newImage(imageHeight,imageStackFiles.count(), imageFormat);
        colorTable.clear();
        int currentProgress = 0;

        ui->imageProgressBar->setValue(currentProgress);
        ui->imageProgressBar->setMaximum(imageStackFiles.count());

        if (isGrayScale && imageFormat == QImage::Format_Indexed8) {
            colorTable = colorTableGray;
            newImage.setColorTable(colorTable);
        }

        // Slice to read from
        for (int z = 0; z < imageStackFiles.count(); z++) // loop #2
        {
            // Pixel color to read an copy to new image
            for(int y = 0; y < imageHeight; y++) // loop #3
            {
                QRgb currentPixelColor = chunkCacheList[z][xList][y];

                // 8-bit Images
                if (imageFormat == QImage::Format_Indexed8){
                    // Check if pixel color is already in the new color table
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
            } // end loop #3

            // Update Image Progress Bar
            currentProgress++;
            ui->imageProgressBar->setValue(currentProgress);
            qApp->processEvents();

        } // end loop #2
        xList++;

        // New Filename
        QString filename = QString("%1").arg(currentTotalNumber);
        if(filename.size() == 1) {
            filename.prepend("000");
        } else if (filename.size() == 2) {
            filename.prepend("00");
        } else if (filename.size() == 3) {
            filename.prepend("0");
        }
        filename.append(getOutputExtension());

        // Save new file
        newImage.save(
                    outputToDirectory+"/"+filename,
                    getOutputFormat()
                    );

        // Update Total Progress Bar
        currentTotalNumber++;
        qDebug() << "Created Slice " << currentTotalNumber << "from Chunk.";
        ui->totalProgressBar->setValue(currentTotalNumber);
        qApp->processEvents();
    } // end loop #1
}

void MainWindow::transmogrifierLoadOneCopyRow()
{
    int currentImageNumber = 0;

    if (selectedDirection == X0toXn) {
        ui->totalProgressBar->setMaximum(imageWidth);

        // Current x value for reading
        for(int x = 0; x < imageWidth; x++)  // loop #1
        {

            // Create new .bmp file to write to
            QImage newImage(imageHeight, imageStackFiles.count(), imageFormat);
            QVector<QRgb> colorTable;
            int currentProgress = 0;

            ui->imageProgressBar->setValue(currentProgress);
            ui->imageProgressBar->setMaximum(imageStackFiles.count());

            // Slice to read from
            for (int z = 0; z < imageStackFiles.count(); z++) // loop #2
            {
                // Open Image z for reading
                QImage image = QImage(inputFromDirectory.absolutePath()+"/"+imageStackFiles[z]);

                // Pixel color to read an copy to new image
                for(int y = 0; y < imageHeight; y++) // loop #3
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
                } // end loop #3

                // Update Image Progress Bar
                currentProgress++;
                ui->imageProgressBar->setValue(currentProgress);
                qApp->processEvents();

            } // end loop #2


            // New Filename
            QString filename = QString("%1").arg(currentImageNumber);
            if(filename.size() == 1) {
                filename.prepend("000");
            } else if (filename.size() == 2) {
                filename.prepend("00");
            } else if (filename.size() == 3) {
                filename.prepend("0");
            }
            filename.append(getOutputExtension());

            // Save new file
            newImage.save(
                        outputToDirectory+"/"+filename,
                        getOutputFormat()
                        );

            // Update Total Progress Bar
            currentImageNumber++;
            ui->totalProgressBar->setValue(currentImageNumber);
            qApp->processEvents();

        } // end loop #1

    } else if (selectedDirection == XntoX0) {
        // TO DO... same as above but in reserve order

    } else if (selectedDirection == Y0toYn) {
        // TO DO... same as X0toXn but with x and y reversed

    } else if (selectedDirection == YntoY0) {
        // TO DO... same as above but in reserve order

    }
}

void MainWindow::abortAction()
{
    transmogrifier->abort();
}

void MainWindow::resetAction()
{
    reset();
}

void MainWindow::aboutAction()
{
    QMessageBox msgBox;
    QSpacerItem* horizontalSpacer = new QSpacerItem(600, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setWindowTitle(tr("About Image Stack Transmogrifier"));
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(
                tr("<b>Image Stack Transmogrifier</b> is a program to create a new image stack at 90 degrees (either along the x or y axis) from a pre-exsisting (numerically named) image stack.")
                +"<br><br>"
                +tr("Image Stack Transmogrifier Version: v%1").arg( QString::number(APP_VERSION, 'f', 1) )
                +"<br><br>"
                +tr("Created by: %1").arg("Alan R.T. Spencer")
                );
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    msgBox.exec();
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


void MainWindow::getImageStackFileList(QDir directory)
{
    directory.setFilter(QDir::Files);
    for(int i = 0; i < directory.entryInfoList().size(); i++)
    {
        imageStackFiles.append(directory.entryInfoList()[i].fileName());
    }
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

bool MainWindow::isCacheEnabled()
{
    return ui->useCache->isChecked();
}

// Progress Bars
void MainWindow::setupChunkProgressBar(int maxValue)
{
    ui->chunkProgressBar->setMaximum(maxValue);
}

void MainWindow::setupSliceProgressBar(int maxValue)
{
    ui->imageProgressBar->setMaximum(maxValue);
}

void MainWindow::setupOverallProgressBar(int maxValue)
{
    ui->totalProgressBar->setMaximum(maxValue);
}

void MainWindow::setChunkProgress(int value)
{
    ui->chunkProgressBar->setValue(value);
}

void MainWindow::setSliceProgress(int value)
{
    ui->imageProgressBar->setValue(value);
}

void MainWindow::setOverallProgress(int value)
{
   ui->totalProgressBar->setValue(value);
}
