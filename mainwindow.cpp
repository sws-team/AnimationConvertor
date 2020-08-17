#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMovie>
#include <QFileDialog>
#include <QPainter>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QtMath>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	movie = new QMovie(this);
	ui->lbl_gif->setMovie(movie);
	this->setAcceptDrops(true);

	connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::closeAllWindows);
	connect(movie, &QMovie::frameChanged, this, &MainWindow::updateFrame);

	ui->sbx_columns->setValue(10);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_button_browse_clicked()
{
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Choose file"),
														  ui->edit_file->text(), tr("Gifs (*.gif)"));
	if (fileName.isEmpty())
		return;

	ui->edit_file->setText(fileName);
	loadFile(fileName);
}

void MainWindow::on_actionConvert_triggered()
{
	const QString fileName = QFileDialog::getSaveFileName(this, tr("Choose file"),
														  QDir::currentPath(), tr("Images (*.png)"));
	if (fileName.isEmpty())
		return;

	convertFile(fileName);
}

void MainWindow::on_edit_file_editingFinished()
{
	loadFile(ui->edit_file->text());
}

void MainWindow::dropEvent(QDropEvent *event)
{
	if (!event->mimeData()->hasUrls())
		return;

	const QList<QUrl> urlList = event->mimeData()->urls();
	if (urlList.isEmpty())
		return;

	const QString fileName = urlList.first().toLocalFile();
	ui->edit_file->setText(fileName);
	loadFile(fileName);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}

void MainWindow::loadFile(const QString &fileName)
{
	if (!QFile::exists(fileName))
		return;

	movie->stop();
	movie->setFileName(fileName);
	ui->horizontalSlider->setRange(0, movie->frameCount());
	movie->start();
	ui->lbl_size->setText(tr("Size: %1x%2").arg(movie->frameRect().width()).arg(movie->frameRect().height()));
	ui->sbx_first->setMaximum(movie->frameCount());
	ui->sbx_last->setMaximum(movie->frameCount());
	ui->sbx_first->setValue(0);
	ui->sbx_last->setValue(movie->frameCount());
}

bool MainWindow::convertFile(const QString &fileName)
{
	if (!movie->isValid())
		return false;

	const int firstFrame = ui->sbx_first->value();
	const int lastFrame = ui->sbx_last->value();

	if (firstFrame >= lastFrame)
	{
		QMessageBox::critical(this, this->windowTitle(), tr("First frame must be more then last frame!"));
		return false;
	}

	const int maxColumns = ui->sbx_columns->value();
	const int frameCount = lastFrame - firstFrame;
	const int movieFrameCount = movie->frameCount();

	constexpr int MAX_SIZE = 32768;
	writeToLog("----------------------------------------------");
	writeToLog(tr("File: %1").arg(movie->fileName()));
	if (frameCount % maxColumns != 0)
		writeToLog(tr("End frames will be cut!"));
	writeToLog(tr("Frames from %1 to %2").arg(firstFrame).arg(lastFrame));
	writeToLog(tr("Converting...."));
	movie->stop();
	const QSize frameSize = QSize(movie->frameRect().width(),
								  movie->frameRect().height());
	const int rows = frameCount / maxColumns;
	const QSize imageSize = QSize(frameSize.width() * maxColumns,
								  frameSize.height() * rows);
	if (imageSize.width() > MAX_SIZE || imageSize.height() > MAX_SIZE)
	{
		writeToLog(tr("Image size over %1x%1").arg(MAX_SIZE));
		return false;
	}
	writeToLog(tr("Frames count %1").arg(frameCount));
	QPixmap pixmap(imageSize);
	QPainter painter(&pixmap);
	int currentRow = 0;
	int currentColumn = 0;
	for (int frame = 0; frame < movieFrameCount; ++frame)
	{
		movie->jumpToFrame(frame);
		if (frame < firstFrame || frame > lastFrame)
			continue;

		const QPixmap framePixmap = movie->currentPixmap();
		const QRectF targetRect = QRectF(frameSize.width() * currentColumn,
										 frameSize.height() * currentRow,
										 frameSize.width(), frameSize.height());
		const QRectF sourceRect = QRectF(0, 0, frameSize.width(), frameSize.height());
		painter.drawPixmap(targetRect, framePixmap, sourceRect);
		currentColumn++;
		if (currentColumn >= maxColumns)
		{
			currentRow++;
			currentColumn = 0;
		}
	}
	writeToLog(tr("Columns count %1").arg(maxColumns));
	writeToLog(tr("Rows count %1").arg(currentRow));
	movie->start();
	writeToLog(tr("Saving..."));
	const bool result = pixmap.save(fileName, "png");
	writeToLog(tr("Finished: %1").arg(result));
	return result;
}

void MainWindow::writeToLog(const QString &text)
{
	ui->edit_log->append(text);
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::information(this, tr("About"), this->windowTitle() + tr("\nCreated by SWS TEAM\n2020"));
}

void MainWindow::updateFrame(int n)
{
	ui->lbl_currentFrame->setText(tr("Frame: %1").arg(n));
	if (ui->horizontalSlider->value() != n)
		ui->horizontalSlider->setValue(n);
}
