
#include "stdafx.h"
#include "Dev1.h"

extern int messageIfNegative(char *str, bool zeroOK);	// Dev2.cpp

Dev1::Dev1() {
	mainDlg = NULL;
}

bool Dev1::loadFromFrontEnd(CMyProc2Dlg *mainDlg) {
	char buffer[100];

	mainDlg->GetDlgItem(IDC_COMMAND24)->GetWindowText(buffer, 100);
	inSpeed = messageIfNegative(buffer, false);

	mainDlg->GetDlgItem(IDC_COMMAND25)->GetWindowText(buffer, 100);
	outSpeed = messageIfNegative(buffer, false);

	if (inSpeed == -1 || outSpeed == -1)
		return false;

	mainDlg->GetDlgItem(IDC_COMMAND28)->GetWindowText(buffer, 100);
	airStep = messageIfNegative(buffer, false);
	if (airStep == -1)
		return false;
	if (airStep > Max_Step) {
		CString c;
		c.Format("单边管长不能超过%d", Max_Step);
		AfxMessageBox(c);
		return false;
	}

	return true;
}

bool Dev1::init(CMyProc2Dlg *mainDlg, int devID) {
	if (!loadFromFrontEnd(mainDlg))
		return false;

	if (this->mainDlg != NULL)
		return true;

	this->mainDlg = mainDlg;
	//this->devID = devID;

	curStep = 0;
	inLeft = true;

	char buf[5];
	precommand = "/";
	precommand += itoa(devID, buf, 10);

	if (runCommand(precommand + "ZR") == false) {
		this->mainDlg = NULL;
		return false;
	}
	return true;
}

/*bool Dev1::hasInited() {
	return mainDlg != NULL;
}*/

bool Dev1::runCommand(CString command) {
	return mainDlg->runCommandDev1(command, command.GetLength());
}

int Dev1::getPosition() {
	return curStep;
}

bool Dev1::goToPos(int position) {
	CString c;
	if (position > Max_Step) {
		c.Format("Warning! Attempt to move to position=%d bigger than max step, current position=%d", 
			position, getPosition());
		AfxMessageBox(c);

		position = Max_Step;
	}
	if (position < 0) {
		c.Format("Warning! Attempt to move to negative position=%d, current position=%d", 
			position, getPosition());
		AfxMessageBox(c);

		position = 0;
	}

	char bufPosition[10], bufInSpeed[10], bufOutSpeed[10];
	itoa(position, bufPosition, 10);
	itoa(inSpeed, bufInSpeed, 10);
	itoa(outSpeed, bufOutSpeed, 10);

	bool ret;
	if (curStep > position)		// out
		ret = runCommand(precommand + "V" + bufOutSpeed + "A" + bufPosition + "R");
	else
		ret = runCommand(precommand + "V" + bufInSpeed + "A" + bufPosition + "R");
	curStep = position;
	return ret;
}

bool Dev1::goLeft() {
	bool ret = true;
	if (!inLeft) {
		ret = runCommand(precommand + "IR");
		inLeft = true;
	}
	return ret;
}

bool Dev1::goRight() {
	bool ret = true;
	if (inLeft) {
		ret = runCommand(precommand + "OR");
		inLeft = false;
	}
	return ret;
}

int Dev1::eject(int step, bool useMin) {
	int min = 0, ejected = 0;
	if (useMin)
		min = Min_Step;
	if (curStep >= step + min)
		ejected = step;
	else if (curStep > min)
		ejected = curStep - min;

	//step -= ejected;
	goToPos(curStep - ejected);
	return ejected;
}

bool Dev1::canEject(int step, bool useMin) {
	int min = 0;
	if (useMin)
		min = Min_Step;
	if (curStep >= step + min)
		return true;
	return false;
}

/*
void Dev1::ejectTwoSide(int step, bool useMin) {
	int min = 0;
	if (useMin)
		min = minStep;
	while (step > 0) {
		if (curStep >= step + min) {
			goRight();
			goToPos(curStep - step);
			step = 0;
		} else if (curStep > min) {
			goRight();
			step -= (curStep - min);
			goToPos(min);
		} else {
			goLeft();
			goToPos(maxStep);
		}
	}
	return;
}
*/

