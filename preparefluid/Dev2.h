
#ifndef DEV2
#define DEV2

#include "MyProc2Dlg.h"

// Injector class
class Dev2 {
public:
	CMyProc2Dlg *mainDlg;
	int xStepN[2], yStepN[2];
	int xStepSize[2], yStepSize[2];
	int zMovePosition[2], zEjectPosition[2], zInhalePosition[2];

	int xWater1, yWater1;	// in id 0 (left)
	int xWater2, yWater2;	// in id 0 (left)
	int xWater3, yWater3;	// in id 0 (left)
	int xWaste, yWaste;		// in id 1 (right)

	int px[2], py[2], pz[2];

	Dev2();
	bool loadFromFrontEnd(CMyProc2Dlg *mainDlg);
	bool init(CMyProc2Dlg *mainDlg);

	bool issueCommand(int id, int posX, int posY, int posZ);
	bool moveToPosition(int id, int posX, int posY, int posZ);
	bool moveToPosition(int id, char *str, int n);
	bool moveToIndex(int id, int indexX, int indexY, int posZ);
	
	bool moveToAir();			// id 0
	bool moveToInhaleWater();	// id 0
	bool moveToEjectWaste();	// id 1
	bool moveToInhaleInput(int n);
	bool moveToEjectOutput(int n);
};

#endif
