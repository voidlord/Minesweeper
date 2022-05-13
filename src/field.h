#ifndef FIELD_H
#define FIELD_H

#include <QPushButton>
#include <QMouseEvent>

using namespace std;

class Field : public QPushButton {
	Q_OBJECT;

	int x;
	int y;
	bool isMine;
	int neighbourMines;
	int isVisible;

	bool flagged;

public:
	Field(int x, int y);

	int getX();
	int getY();
	bool getMineStatus();
	void setMineStatus(bool isMine);
	int getNeighbourMines();
	void setNeighbourMines(int numOfMines);
	void show();
	bool isFlagged();
	void setFlag(bool status);

	void mousePressEvent(QMouseEvent* e);

signals:
	void rightClicked();
	void middleClicked();
};

#endif // FIELD_H
