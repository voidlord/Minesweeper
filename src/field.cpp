#include "field.h"

Field::Field(int x, int y) :
	x(x),
	y(y),
	isMine(false),
	neighbourMines(0),
	isVisible(false),
	flagged(false),
	enabled(true)
{

}

int Field::getX() {
	return this->x;
}

int Field::getY() {
	return this->y;
}

bool Field::getMineStatus() {
	return this->isMine;
}

void Field::setMineStatus(bool isMine) {
	this->isMine = isMine;
}

int Field::getNeighbourMines() {
	return this->neighbourMines;
}

void Field::setNeighbourMines(int numOfMines) {
	this->neighbourMines = numOfMines;
}

void Field::show() {
	if (this->neighbourMines > 0) {
		this->setText(QString::number(this->neighbourMines));
	} else {
		this->setText("");
	}
	this->setStyleSheet("color: rgb(120, 120, 120); background-color: rgb(204, 204, 204); border: 1px solid #FFFFFF;");

	this->flagged = false;
	this->enabled = false;
}

bool Field::isFlagged() {
	return this->flagged;
}

bool Field::isEnabled() {
	return this->enabled;
}

void Field::setFlag(bool status) {
	this->flagged = status;
}

void Field::mousePressEvent(QMouseEvent* e) {
	if (this->enabled == true) {
		if (e->button() == Qt::RightButton) {
			emit rightClicked();
		} else if (e->button() == Qt::LeftButton) {
			emit pressed();
		}
	} else if (e->button() == Qt::MiddleButton) {
		emit middleClicked();
	}
}
