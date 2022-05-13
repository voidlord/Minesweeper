#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>

#include <vector>

#include "field.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace std;

struct Settings {
	int Width = 9;
	int Height = 9;
	int Mines = 10;
	int buttonSize = 40;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void generateLayout();
	void generateFields(int startX, int startY);
	void setNumbersForFields();
	void endGame(string resultText);
	void clearField();

	void attemptMultiClear(int i, int j);

private:
	Ui::MainWindow *ui;

	struct Settings settings;

	bool lockButtons;

	int windowXsize;
	int windowYsize;

	bool firstPress;

	int mineCounter;

	vector<vector<Field*>> fields;
	QWidget* fieldWidget;
	QGridLayout* gridLayout;

	void clearEmptyAux(int i, int j);
};
#endif // MAINWINDOW_H
