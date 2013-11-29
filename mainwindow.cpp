#include "commonheader.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    log = new Logger(this);
    transmogrifier = new Transmogrifier(this, log);

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
    connect(ui->outputFormatComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setOutputFormat(int)));
    connect(ui->outputToButton, SIGNAL(clicked()), this, SLOT(outputToAction()));
    connect(this, SIGNAL(dataChanged()), this, SLOT(checkRunButton()));
    connect(ui->runButton, SIGNAL(clicked()), this, SLOT(runAction()));
    connect(ui->abortButton, SIGNAL(clicked()), this, SLOT(abortAction()));
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(resetAction()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutAction()));
    connect(ui->saveLogButton, SIGNAL(clicked()), this, SLOT(saveLogAction()));
    connect(ui->clearLogButton, SIGNAL(clicked()), this, SLOT(clearLogAction()));

    log->append("Main Application", "Intiallised");
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

    ui->algorithmComboBox->setCurrentIndex(0);
    ui->cacheChuckSize->setValue(50);

    ui->inputFromButton->setEnabled(true);
    ui->directionX0toXn->setEnabled(true);
    ui->directionXntoX0->setEnabled(true);
    ui->directionY0toYn->setEnabled(true);
    ui->directionYntoY0->setEnabled(true);
    ui->outputToButton->setEnabled(true);
    ui->runButton->setEnabled(false);
    ui->abortButton->setEnabled(false);
    ui->resetButton->setEnabled(true);

    ui->totalProgressBar->setValue(0);
    ui->imageProgressBar->setValue(0);
    ui->chunkProgressBar->setValue(0);
}

void MainWindow::inputFromAction()
{
    log->append("X/Y Transmogrifier", "Opening Image Stack folder selection window.");
    inputFromFilename = QFileDialog::getOpenFileName(
                this,
                tr("Select First Image of Stack"),
                inputFromDirectory.absolutePath(),
                tr("Image Files (*.bmp *.tif *.tif *.jpeg *.jpg *.png)")
                );
    if (!inputFromFilename.isEmpty()) {
        inputFromDirectory = QFileInfo(inputFromFilename).absoluteDir();
        ui->inputFrom->setText(inputFromDirectory.absolutePath());
        log->append("X/Y Transmogrifier",
                QString("Input Image Stack folder = \"%1\".")
                    .arg(inputFromDirectory.absolutePath())
                );

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
            log->append("X/Y Transmogrifier",
                    QString("Cannot open %1. Only the following file formates can be opened: %2.")
                        .arg(inputFromFilename)
                        .arg(getAvailableFormatsStr())
                    );
            return;
        } else {           
            inputFromOK = true;

            log->append("X/Y Transmogrifier",
                    QString("Reading %1 and collating image information.")
                        .arg(inputFromFilename)
                    );

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
        log->append("X/Y Transmogrifier", "No folder/file selected.");
    }
    log->append("X/Y Transmogrifier", "Closing Image Stack folder selection window.");
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
   QString selected =  ui->outputFormatComboBox->itemText(index);

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

// Log Dock
void MainWindow::logAppend(QString message)
{
    ui->logBrowser->append(message);
}

void MainWindow::logClear()
{
    ui->logBrowser->clear();
}

void MainWindow::saveLogAction()
{
    QString filename = QFileDialog::getSaveFileName(
            this,
            tr("Save Application Log"),
            "",
            tr("Text File (*.txt);;All Files (*)")
            );
    if (filename.isEmpty()) {
             return;
    } else {
        log->outputToFile(filename);
    }
}

void MainWindow::clearLogAction(){
    log->clear();
}
