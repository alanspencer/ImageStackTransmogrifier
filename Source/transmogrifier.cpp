#include "transmogrifier.h"

Transmogrifier::Transmogrifier(MainWindow *mw)
{
    mainWindow = mw;

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

void Transmogrifier::setXValues(int value, int start = 0, int end = 0)
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

void Transmogrifier::setXValues(int value, int start = 0, int end = 0)
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
        if (end > yValue || end < 0 || ) {
            // Throw an exception

        } else {
            yEndValue = end;
        }
    } else {
        yEndValue = yValue;
    }
}

void Transmogrifier::setZValues(int value, int start = 0, int end = 0)
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
        if (end > zValue || end < 0 || ) {
            // Throw an exception
            throw Exception("Value of zEnd must be greater than zStart, not be 0, and less than z.");
        }
        zEndValue = end;
    } else {
        zEndValue = zValue;
    }
}

void Transmogrifier::setFileStructure(QString inDir, QList<QString> fList, QString outDir)
{
    inputFromDirectory = inDir;
    outputToDirectory = outDir;
    inputStackFiles = fList;
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

bool

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

    // Workout values for progress bars
    if (direction = X0toXn || direction = XntoX0) {
        // overall number of slices with be along x axis
        overallMaxValue = xEndValue-xStartValue;
    } else if (direction = Y0toYn || direction = YntoY0) {
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
const char* Transmogrifier::getOutputFormat() {
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

const char* Transmogrifier::getOutputExtension() {
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
