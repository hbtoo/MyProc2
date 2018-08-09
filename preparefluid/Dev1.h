
#ifndef DEV1
#define DEV1

#include "MyProc2Dlg.h"

// Injector constants
const int Min_Step = 50;
const int Max_Step = 5000;

// Injector class
class Dev1 {
public:
	CMyProc2Dlg *mainDlg;
	CString precommand;
	//int devID;

	int curStep;
	bool inLeft;

	int inSpeed;
	int outSpeed;

	int airStep;

	Dev1();
	bool loadFromFrontEnd(CMyProc2Dlg *mainDlg);
	bool init(CMyProc2Dlg *mainDlg, int devID);
	//bool hasInited();
	bool runCommand(CString command);
	int getPosition();
	bool goToPos(int position);
	bool goLeft();
	bool goRight();
	//void ejectTwoSide(int step, bool useMin);
	int eject(int step, bool useMin);
	bool canEject(int step, bool useMin);
};

#endif
