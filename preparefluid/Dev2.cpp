
#include "stdafx.h"
#include "Dev2.h"
#include "math.h"

Dev2::Dev2() {
	mainDlg = NULL;
}

/*
CString c;
c.Format("haha %d!", n);
mainDlg->GetDlgItem(IDC_COMMAND1)->SetWindowText(c);
*/

int messageIfNegative(char *str, bool zeroOK) {
	int n = atoi(str);
	CString c = "参数";
	c += str;
	if (n < 0) {
		c += "应为非负整数";
		AfxMessageBox(c);
		return -1;
	} else if (n == 0 && !zeroOK) {
		c += "应为正整数";
		AfxMessageBox(c);
		return -1;
	}
	return n;
}

bool Dev2::loadFromFrontEnd(CMyProc2Dlg *mainDlg) {
	char buffer[100];

	mainDlg->GetDlgItem(IDC_COMMAND3)->GetWindowText(buffer, 100);
	xStepN[0] = messageIfNegative(buffer, false);
	mainDlg->GetDlgItem(IDC_COMMAND4)->GetWindowText(buffer, 100);
	yStepN[0] = messageIfNegative(buffer, false);
	
	xStepSize[0] = yStepSize[0] = 1;
	if (xStepN[0] > 1) {
		mainDlg->GetDlgItem(IDC_COMMAND5)->GetWindowText(buffer, 100);
		xStepSize[0] = messageIfNegative(buffer, false);
		if (xStepSize[0] == -1)
			return false;
		xStepSize[0] = xStepSize[0] / (xStepN[0] - 1);
	}
	if (yStepN[0] > 1) {
		mainDlg->GetDlgItem(IDC_COMMAND6)->GetWindowText(buffer, 100);
		yStepSize[0] = messageIfNegative(buffer, false);
		if (yStepSize[0] == -1)
			return false;
		yStepSize[0] = yStepSize[0] / (yStepN[0] - 1);
	}

	mainDlg->GetDlgItem(IDC_COMMAND7)->GetWindowText(buffer, 100);
	zMovePosition[0] = messageIfNegative(buffer, true);
	mainDlg->GetDlgItem(IDC_COMMAND8)->GetWindowText(buffer, 100);
	zEjectPosition[0] = messageIfNegative(buffer, true);
	mainDlg->GetDlgItem(IDC_COMMAND13)->GetWindowText(buffer, 100);
	zInhalePosition[0] = messageIfNegative(buffer, true);

	mainDlg->GetDlgItem(IDC_COMMAND9)->GetWindowText(buffer, 100);
	xWater1 = messageIfNegative(buffer, true);
	mainDlg->GetDlgItem(IDC_COMMAND10)->GetWindowText(buffer, 100);
	yWater1 = messageIfNegative(buffer, true);

	mainDlg->GetDlgItem(IDC_COMMAND21)->GetWindowText(buffer, 100);
	xWater2 = messageIfNegative(buffer, true);
	mainDlg->GetDlgItem(IDC_COMMAND22)->GetWindowText(buffer, 100);
	yWater2 = messageIfNegative(buffer, true);

	mainDlg->GetDlgItem(IDC_COMMAND26)->GetWindowText(buffer, 100);
	xWater3 = messageIfNegative(buffer, true);
	mainDlg->GetDlgItem(IDC_COMMAND27)->GetWindowText(buffer, 100);
	yWater3 = messageIfNegative(buffer, true);

	if (xStepN[0] == -1 || yStepN[0] == -1 || zMovePosition[0] == -1 
		|| zEjectPosition[0] == -1 || zInhalePosition[0] == -1
		|| xWater1 == -1 || yWater1 == -1 || xWater2 == -1 || yWater2 == -1
		|| xWater3 == -1 || yWater3 == -1)
		return false;



	mainDlg->GetDlgItem(IDC_COMMAND15)->GetWindowText(buffer, 100);
	xStepN[1] = messageIfNegative(buffer, false);
	mainDlg->GetDlgItem(IDC_COMMAND16)->GetWindowText(buffer, 100);
	yStepN[1] = messageIfNegative(buffer, false);
	
	xStepSize[1] = yStepSize[1] = 1;
	if (xStepN[1] > 1) {
		mainDlg->GetDlgItem(IDC_COMMAND17)->GetWindowText(buffer, 100);
		xStepSize[1] = messageIfNegative(buffer, false);
		if (xStepSize[1] == -1)
			return false;
		xStepSize[1] = xStepSize[1] / (xStepN[1] - 1);
	}
	if (yStepN[1] > 1) {
		mainDlg->GetDlgItem(IDC_COMMAND18)->GetWindowText(buffer, 100);
		yStepSize[1] = messageIfNegative(buffer, false);
		if (yStepSize[1] == -1)
			return false;
		yStepSize[1] = yStepSize[1] / (yStepN[1] - 1);
	}

	mainDlg->GetDlgItem(IDC_COMMAND19)->GetWindowText(buffer, 100);
	zMovePosition[1] = messageIfNegative(buffer, true);
	mainDlg->GetDlgItem(IDC_COMMAND20)->GetWindowText(buffer, 100);
	zEjectPosition[1] = messageIfNegative(buffer, true);
	mainDlg->GetDlgItem(IDC_COMMAND23)->GetWindowText(buffer, 100);
	zInhalePosition[1] = messageIfNegative(buffer, true);

	mainDlg->GetDlgItem(IDC_COMMAND11)->GetWindowText(buffer, 100);
	xWaste = messageIfNegative(buffer, true);
	mainDlg->GetDlgItem(IDC_COMMAND12)->GetWindowText(buffer, 100);
	yWaste = messageIfNegative(buffer, true);

	if (xStepN[1] == -1 || yStepN[1] == -1 || zMovePosition[1] == -1 
		|| zEjectPosition[1] == -1 || zInhalePosition[1] == -1
		|| xWaste == -1 || yWaste == -1)
		return false;

	return true;
}

bool Dev2::init(CMyProc2Dlg *mainDlg) {
	if (!loadFromFrontEnd(mainDlg))
		return false;

	if (this->mainDlg != NULL)
		return true;

	this->mainDlg = mainDlg;
	px[0] = py[0] = pz[0] = px[1] = py[1] = pz[1] = 0;
	return true;
}

bool Dev2::issueCommand(int id, int posX, int posY, int posZ) {
	CString c;
	c.Format("%d %d %d", posX, posY, posZ);
	if (id == 0)
		return mainDlg->runCommandDev2(c, c.GetLength());
	return mainDlg->runCommandDev3(c, c.GetLength());
}

bool Dev2::moveToPosition(int id, int posX, int posY, int posZ) {
	if (posX == px[id] && posY == py[id] && posZ == pz[id])
		return true;

	// !!! The arduino code always move x, y first !!!
	if ((posX != px[id] || posY != py[id]) && pz[id] > zMovePosition[id]) {
		if (issueCommand(id, px[id], py[id], zMovePosition[id]) == false)
			return false;
		//Sleep(2000);	// wait for potential dripping
	}
	// save the position first, so that if thread killed half way executing, 
	// this info is there, so that move to 0, 0, 0 would be safe
	px[id] = posX;
	py[id] = posY;
	pz[id] = posZ;

	if (issueCommand(id, posX, posY, posZ) == false)
		return false;

	return true;
}

bool Dev2::moveToPosition(int id, char *str, int n) {
	int x, y, z;
	sscanf(str, "%d %d %d", &x, &y, &z);
	return moveToPosition(id, x, y, z);
}

bool Dev2::moveToIndex(int id, int indexX, int indexY, int posZ) {
	if (indexX < 0 || indexX >= xStepN[id] || indexY < 0 || indexY >= yStepN[id])
		return false;
	return moveToPosition(id, indexX * xStepSize[id], indexY * xStepSize[id], posZ);
}

bool Dev2::moveToAir() {
	int id = 0;
	if (pz[id] <= zMovePosition[id])
		return true;
	return moveToPosition(id, px[id], py[id], zMovePosition[id]);
}

bool Dev2::moveToInhaleWater() {
	int id = 0;
	if (xWater3 == px[id] && yWater3 == py[id] && zInhalePosition[id] == pz[id])
		return true;

	if (!moveToPosition(id, xWater1, yWater1, zInhalePosition[id]))
		return false;
	Sleep(500);

	if (!moveToPosition(id, xWater2, yWater2, zInhalePosition[id]))
		return false;
	Sleep(500);

	return moveToPosition(id, xWater3, yWater3, zInhalePosition[id]);
}

bool Dev2::moveToEjectWaste() {
	int id = 1;
	return moveToPosition(id, xWaste, yWaste, zMovePosition[id]);
}

bool Dev2::moveToInhaleInput(int n) {
	int id = 0;
	int x = n / yStepN[id];
	int y = n % yStepN[id];
	return moveToIndex(id, x, y, zInhalePosition[id]);
}

bool Dev2::moveToEjectOutput(int n) {
	int id = 1;
	int x = n / yStepN[id];
	int y = n % yStepN[id];
	return moveToIndex(id, x, y, zEjectPosition[id]);
}
