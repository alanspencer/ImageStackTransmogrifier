#include "commonheader.h"
#include "ui_transmogrifier.h"

Transmogrifier::Transmogrifier(QWidget *parent):
    QWidget(parent),
    form(new Ui::transmogrifierForm)
{
    form->setupUi(this);

    direction = X0toXn;
    codeVersion = StandardCode;
    outputFormat = BMPFormat;

    reset();
}

void Transmogrifier::reset()
{
    running = false;
    zValue = 0;
    xValue = 0;
    yValue = 0;
    zStartValue = 0;
    zEndValue = 0;
    xStartValue = 0;
    xEndValue = 0;
    yStartValue = 0;
    yEndValue = 0;
    chunkSize = 50;
    isGrayscale = false;

    createGrayscaleColorTable();
}

// Setters

void Transmogrifier::setDirection(Direction dir = X0toXn)
{
    direction = dir;
}

void Transmogrifier::setCodeVersion(CodeVersion cVer = StandardCode)
{
    codeVersion = cVer;
}

void Transmogrifier::setOutputFormat(OutputFormat of)
{
    outputFormat = of;
}

void Transmogrifier::setXValues(int value, int start, int end)
{
    // x value
    if (value <= 0) {
        // Throw an exception. x must be greater than 0.
        throw Exception("Value of x must be greater than 0.");
    } else {
        xValue = value;
    }

    // x start value
    if (start > xValue || start < 0) {
        // Throw an exception
        throw Exception("Value of xStart must be greater than 0 and less than x.");
    } else {
        xStartValue = start;
    }

    // x end value
    if (end != 0) {
        if (end > xValue || end < 0 || end <= xStartValue) {
            // Throw an exception
            throw Exception("Value of xEnd must be greater than xStart, not be 0, and less than x.");
        } else {
            xEndValue = end;
        }
    } else {
        xEndValue = xValue;
    }
}

void Transmogrifier::setYValues(int value, int start, int end)
{
    // y value
    if (value <= 0) {
        // Throw an exception. y must be greater than 0.
        throw Exception("Value of y must be greater than 0.");
    } else {
        yValue = value;
    }

    // y start value
    if (start > yValue || start < 0) {
        // Throw an exception
        throw Exception("Value of yStart must be greater than 0 and less than y.");
    } else {
        yStartValue = start;
    }

    // x end value
    if (end != 0) {
        if (end > yValue || end < 0 || end <= yStartValue) {
            // Throw an exception

        } else {
            yEndValue = end;
        }
    } else {
        yEndValue = yValue;
    }
}

void Transmogrifier::setZValues(int value, int start, int end)
{
    // z value
    if (value <= 0) {
        // Throw an exception. x must be greater than 0.
        throw Exception("Value of z must be greater than 0.");
    } else {
        zValue = value;
    }

    // z start value
    if (start > zValue || start < 0) {
        // Throw an exception
        throw Exception("Value of zStart must be greater than 0 and less than z.");
    } else {
        zStartValue = start;
    }

    // z end value
    if (end != 0) {
        if (end > zValue || end < 0 || end <= zStartValue) {
            // Throw an exception
            throw Exception("Value of zEnd must be greater than zStart, not be 0, and less than z.");
        }
        zEndValue = end;
    } else {
        zEndValue = zValue;
    }
}

bool Transmogrifier::setInputDirectory(QString inDir)
{
    inputFromDirectory = inDir;

    // Run File Stack checks
    populateInputFileList();

    // Update GUI if OK
    return true;
}

bool Transmogrifier::setOutputDirectory(QString outDir)
{
    outputToDirectory = outDir;

    // update GUI if OK
    return true;
}

void Transmogrifier::setChunkSize(int size)
{
    if (size <= 0) {
        // Throw exception
        throw Exception("Chunk Size can not be set to 0.");
    }
    chunkSize = size;
}

void Transmogrifier::setImageFormat(QImage::Format format)
{
    imageFormat = format;
}

void Transmogrifier::setIsGrayscale(bool grayscale)
{
    isGrayscale = grayscale;
}

// Getters

Transmogrifier::Direction Transmogrifier::getDirection()
{
    return direction;
}

Transmogrifier::CodeVersion Transmogrifier::getCodeVersion()
{
    return codeVersion;
}

Transmogrifier::OutputFormat Transmogrifier::getOutputFormat()
{
    return outputFormat;
}

QImage::Format Transmogrifier::getImageFormat()
{
    return imageFormat;
}

bool Transmogrifier::getRunningStatus()
{
    return running;
}

bool Transmogrifier::getIsGrayscale()
{
   return isGrayscale;
}

int Transmogrifier::getInputFilesCount()
{
    return inputDirectoryFiles.count();
}


void Transmogrifier::populateInputFileList()
{
    inputDirectoryFiles.clear();
    QDir dir(inputFromDirectory);
    dir.setFilter(QDir::Files);
    for(int i = 0; i < dir.entryInfoList().size(); i++)
    {
        inputDirectoryFiles.append(dir.entryInfoList()[i].fileName());
    }
}

// Run

void Transmogrifier::run()
{

}

// Abort

void Transmogrifier::abort()
{
    running = false;
}

// Progress Bars
void Transmogrifier::setupProgressBars()
{
    int chunkMaxValue = zEndValue-zStartValue; // Should be the zEnd-zStart.
    int sliceMaxValue = chunkMaxValue; // should same as chunkMaxValue
    int overallMaxValue = 0;

    // Workout values for progress bars
    if (direction == X0toXn || direction == XntoX0) {
        // overall number of slices with be along x axis
        overallMaxValue = xEndValue-xStartValue;
    } else if (direction == Y0toYn || direction == YntoY0) {
        //overall number of slices will be along the y axis
        overallMaxValue = yEndValue-yStartValue;
    }

    // Set the values in the MainWindow
    mainWindow->setupChunkProgressBar(chunkMaxValue);
    mainWindow->setupSliceProgressBar(sliceMaxValue);
    mainWindow->setupOverallProgressBar(overallMaxValue);
}

void Transmogrifier::updateChuckProgress(int value)
{
    mainWindow->setChunkProgress(value);
}

void Transmogrifier::updateSliceProgress(int value)
{
    mainWindow->setSliceProgress(value);
}

void Transmogrifier::updateOverallProgress(int value)
{
    mainWindow->setOverallProgress(value);
}

// Color Tables
void Transmogrifier::createGrayscaleColorTable()
{
    // Create grayscale color table
    for(int c = 0; c < 255; c++)
    {
        colorTableGray.append(QColor(c,c,c).rgb());
    }
}

// Output formats
QString Transmogrifier::getOutputFormatText() {
    if (outputFormat == BMPFormat) {
        return "BMP";
    } else if (outputFormat == JPEGFormat) {
        return "JPEG";
    } else if (outputFormat == TIFFFormat) {
        return "TIFF";
    } else if (outputFormat == PNGFormat) {
        return "PNG";
    } else {
        return "BMP";
    }
}

QString Transmogrifier::getOutputExtension() {
    if (outputFormat == BMPFormat) {
        return ".bmp";
    } else if (outputFormat == JPEGFormat) {
        return ".jpeg";
    } else if (outputFormat == TIFFFormat) {
        return ".tiff";
    } else if (outputFormat == PNGFormat) {
        return ".png";
    } else {
        return ".bmp";
    }
}

// Input Format
QString Transmogrifier::getImageFormatText()
{
    QString str;

    // Image Format Text
    if (imageFormat == QImage::Format_Invalid) {
        str = "The images are invalid";
    } else if (imageFormat == QImage::Format_Mono) {
        str = "Monocrome MSB";
    } else if (imageFormat == QImage::Format_MonoLSB) {
        str = "Monocrome LSB";
    } else if (imageFormat == QImage::Format_Indexed8) {
        str = "Index 8";
    } else if (imageFormat == QImage::Format_RGB32) {
        str = "RGB 32";
    } else if (imageFormat == QImage::Format_ARGB32) {
        str = "ARGB 32";
    } else if (imageFormat == QImage::Format_ARGB32_Premultiplied) {
        str = "ARGB 32 Premultiplied";
    } else {
        str = "Undefined";
    }

    if (isGrayscale) {
        str += " (Grayscale)";
    }

    return str;
}

bool Transmogrifier::getImageFormatIsValid()
{
    // Image Format Valid
    if (imageFormat == QImage::Format_Invalid) {
        return false;
    } else if (imageFormat == QImage::Format_Mono) {
        return false;
    } else if (imageFormat == QImage::Format_MonoLSB) {
        return false;
    } else if (imageFormat == QImage::Format_Indexed8) {
        return true;
    } else if (imageFormat == QImage::Format_RGB32) {
        return true;
    } else if (imageFormat == QImage::Format_ARGB32) {
        return true;
    } else if (imageFormat == QImage::Format_ARGB32_Premultiplied) {
        return false;
    } else {
        return false;
    }
}


/*
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
*/
