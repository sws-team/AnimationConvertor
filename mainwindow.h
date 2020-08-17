#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_button_browse_clicked();

	void on_actionConvert_triggered();

	void on_edit_file_editingFinished();

	void on_actionAbout_triggered();

	void updateFrame(int n);

protected:
	void dropEvent(QDropEvent *event) override;
	void dragEnterEvent(QDragEnterEvent *event) override;

private:
	Ui::MainWindow *ui;

	QMovie *movie;
	void loadFile(const QString& fileName);
	bool convertFile(const QString& fileName);
	void writeToLog(const QString& text);
};
#endif // MAINWINDOW_H
