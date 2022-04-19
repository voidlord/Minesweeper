#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->setWindowTitle("Minesweeper");
	this->setFixedSize(310, 400);
	this->statusBar()->hide();

	this->lockButtons = true;

	this->windowXsize = 500;
	this->windowYsize = 500;

	this->fieldWidget = new QWidget();

	ui->mainMenuLayout->setAlignment(Qt::AlignTop);
	ui->presetsLayout->setAlignment(Qt::AlignTop);

	ui->xSizeEdit->setText(QString::number(this->settings.Width));
	ui->ySizeEdit->setText(QString::number(this->settings.Height));
	ui->minesEdit->setText(QString::number(this->settings.Mines));

	connect(ui->saveSettingsButton, &QPushButton::pressed, [=](){
		bool error = false;

		bool ok;

		QPalette okPalette = ui->xSizeEdit->palette();
		okPalette.setColor(ui->xSizeEdit->foregroundRole(), Qt::black);

		QPalette errorPalette = ui->xSizeEdit->palette();
		errorPalette.setColor(ui->xSizeEdit->foregroundRole(), Qt::red);

		int sizeX = ui->xSizeEdit->text().toInt(&ok);
		if (!ok || (sizeX <= 0)) {
			ui->xSizeEdit->setPalette(errorPalette);
			error = true;
		} else {
			ui->xSizeEdit->setPalette(okPalette);
		}

		int sizeY = ui->ySizeEdit->text().toInt(&ok);
		if (!ok || (sizeY <= 0)) {
			ui->ySizeEdit->setPalette(errorPalette);
			error = true;
		} else {
			ui->ySizeEdit->setPalette(okPalette);
		}

		int mines = ui->minesEdit->text().toInt(&ok);
		if (!ok || (mines >= (sizeX*sizeY))) {
			ui->minesEdit->setPalette(errorPalette);
			error = true;
		} else {
			ui->minesEdit->setPalette(okPalette);
		}

		if (!error) {
			this->settings.Height = sizeY;
			this->settings.Width = sizeX;
			this->settings.Mines = mines;
		}
	});

	connect(ui->easyDiffButton, &QPushButton::pressed, [&](){
		this->settings.Height = 9;
		this->settings.Width = 9;
		this->settings.Mines = 10;

		ui->xSizeEdit->setText(QString::number(this->settings.Width));
		ui->ySizeEdit->setText(QString::number(this->settings.Height));
		ui->minesEdit->setText(QString::number(this->settings.Mines));
	});
	connect(ui->intermediateDiffButton, &QPushButton::pressed, [&](){
		this->settings.Height = 13;
		this->settings.Width = 15;
		this->settings.Mines = 40;

		ui->xSizeEdit->setText(QString::number(this->settings.Width));
		ui->ySizeEdit->setText(QString::number(this->settings.Height));
		ui->minesEdit->setText(QString::number(this->settings.Mines));
	});
	connect(ui->expertDiffButton, &QPushButton::pressed, [&](){
		this->settings.Height = 16;
		this->settings.Width = 30;
		this->settings.Mines = 99;

		ui->xSizeEdit->setText(QString::number(this->settings.Width));
		ui->ySizeEdit->setText(QString::number(this->settings.Height));
		ui->minesEdit->setText(QString::number(this->settings.Mines));
	});

	connect(ui->playButton, &QPushButton::pressed, [&](){
		ui->stackedWidget->addWidget(this->fieldWidget);
		ui->stackedWidget->setCurrentWidget(this->fieldWidget);

		if (this->settings.Width > this->settings.Height) {
			this->windowXsize = ((this->settings.Width*(this->settings.Width/this->settings.Height))/2)*100;
			this->windowYsize = (this->settings.Height/2)*100;
		} else if (this->settings.Width > this->settings.Height) {
			this->windowXsize = (this->settings.Width/2)*100;
			this->windowYsize = ((this->settings.Height*(this->settings.Height/this->settings.Width))/2)*100;
		} else {
			this->windowXsize = (this->settings.Width/2)*100;
			this->windowYsize = (this->settings.Height/2)*100;
		}

		if (this->windowXsize < 400) {
			this->windowXsize = 400;
		}
		if (this->windowYsize < 400) {
			this->windowYsize = 400;
		}

		if (this->windowXsize >= 750 || this->windowYsize >= 750) {
			this->windowXsize /= 1.5;
			this->windowYsize /= 1.5;
		}

		this->generateField();

		this->setFixedSize(this->windowXsize, this->windowYsize);
		ui->stackedWidget->resize(this->windowXsize, this->windowYsize);
		this->setWindowTitle("Mines left: " + QString::number(this->mineCounter));

		this->lockButtons = false;
	});
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::clearEmptyAux(int i, int j) {
	if (this->fields[i][j]->getNeighbourMines() == 0) {
		for (int k = -1; k < 2; k++) {
			for (int l = -1; l < 2; l++) {
				if ((i+k >= 0) && (j+l >= 0) && (i+k < this->settings.Height) && (j+l < this->settings.Width)) {
					if (this->fields[i+k][j+l]->isEnabled() == true) {
						this->fields[i+k][j+l]->setEnabled(false);
						this->fields[i+k][j+l]->show();

						if (this->fields[i+k][j+l]->getNeighbourMines() == 0) {
							clearEmptyAux(i+k, j+l);
						}
					}
				}
			}
		}
	}
}
void MainWindow::generateField() {
	this->fields = vector<vector<Field*>>(this->settings.Height);
	for (int i = 0; i < this->settings.Height; i++) {
		this->fields[i] = vector<Field*>(this->settings.Width);
	}

	this->gridLayout = new QGridLayout();
	this->gridLayout->setSpacing(0);
	this->fieldWidget->setLayout(this->gridLayout);
	for (int i = 0; i < this->settings.Height; i++) {
		for (int j = 0; j < this->settings.Width; j++) {
			Field* field = new Field(i, j);
			field->setText("");
			field->setFont(QFont("Arial", 24));
			field->setMinimumSize(this->size().width()/this->settings.Width, this->size().height()/this->settings.Height);

			connect(field, &Field::pressed, [=](){
				if (this->lockButtons == false && (field->isFlagged() == false)) {
					this->lockButtons = true;

					if (this->firstPress) {
						// Generate mines
						int i = 0;
						while (i < this->settings.Mines) {
							int x, y;
							x = rand() % this->settings.Height;
							y = rand() % this->settings.Width;

							if (x != field->getX() && y != field->getY()) {
								if (this->fields[x][y]->getMineStatus() == false) {
									this->fields[x][y]->setMineStatus(true);

									i++;
								}
							}
						}

						// Count mines
						for (int i = 0; i < this->settings.Height; i++) {
							for (int j = 0; j < this->settings.Width; j++) {
								if (this->fields[i][j]->getMineStatus() == true) {
									for (int k = -1; k < 2; k++) {
										for (int l = -1; l < 2; l++) {
											if ((i+k >= 0) && (j+l >= 0) && (i+k < this->settings.Height) && (j+l < this->settings.Width)) {
												this->fields[i+k][j+l]->setNeighbourMines(this->fields[i+k][j+l]->getNeighbourMines()+1);
											}
										}
									}
								}
							}
						}

						// Debug field
						/*
						for (int i = 0; i < this->settings.Height; i++) {
							for (int j = 0; j < this->settings.Width; j++) {
								if (this->fields[i][j]->getNeighbourMines() > 0) {
									this->fields[i][j]->show();
								}

								if (this->fields[i][j]->getMineStatus() == true) {
									this->fields[i][j]->setText("X");
								}
							}
						}
						//*/

						// Setup a proper start, by clearing all zeroes around start
						{
							int i = field->getX();
							int j = field->getY();
							for (int k = -1; k < 2; k++) {
								for (int l = -1; l < 2; l++) {
									if ((i+k >= 0) && (j+l >= 0) && (i+k < this->settings.Height) && (j+l < this->settings.Width)) {
										if (this->fields[i+k][j+l]->isEnabled() == true) {
											if (this->fields[i+k][j+l]->getNeighbourMines() == 0) {
												this->fields[i+k][j+l]->setEnabled(false);
												this->fields[i+k][j+l]->show();

												this->clearEmptyAux(i+k, j+l);
											}
										}
									}
								}
							}
						}

						this->firstPress = false;
					}

					if (field->getMineStatus() == true) {
						this->endGame("You lost");

						return;
					} else {
						field->setEnabled(false);
						field->show();
					}

					if (field->getNeighbourMines() == 0) {
						this->clearEmptyAux(i, j);
					}

					int enabledCount = 0;
					for (int i = 0; i < this->settings.Height; i++) {
						for (int j = 0; j < this->settings.Width; j++) {
							if (this->fields[i][j]->isEnabled() ==  true) {
								enabledCount++;
							}
						}
					}

					if (enabledCount == this->settings.Mines) {
						this->endGame("You won");

						return;
					}

					this->lockButtons = false;
				}
			});

			connect(field, &Field::rightClicked, [=](){
				if (field->isFlagged() == false) {
					field->setFlag(true);

					field->setStyleSheet("color: rgb(255, 0, 0)");
					field->setText("F");

					this->mineCounter--;
					this->setWindowTitle("Mines left: " + QString::number(this->mineCounter));
				} else {
					field->setFlag(false);

					field->setStyleSheet("");
					field->setText("");

					this->mineCounter++;
					this->setWindowTitle("Mines left: " + QString::number(this->mineCounter));
				}
			});

			this->fields[i][j] = field;
			this->gridLayout->addWidget(field, i, j);
		}
	}

	this->mineCounter = this->settings.Mines;

	this->firstPress = true;
}

void MainWindow::endGame(string resultText) {
	QMessageBox* resultMessage = new QMessageBox();
	resultMessage->setText(QString::fromStdString(resultText));

	resultMessage->exec();

	this->clearField();

	this->setWindowTitle("Minesweeper");
	this->setFixedSize(310, 400);
	ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::clearField() {
	for (int i = 0; i < this->settings.Height; i++) {
		for (int j = 0; j < this->settings.Width; j++) {
			this->fields[i][j]->deleteLater();
			this->fields[i][j] = nullptr;
		}
	}

	this->gridLayout->deleteLater();
	this->gridLayout = nullptr;
}