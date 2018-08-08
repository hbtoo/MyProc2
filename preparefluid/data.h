#ifndef DATA
#define DATA

#include "MyProc2Dlg.h"


class Data {
public:
	int nInput, nOutput;
	double *inArray;
	int **outArray;

	bool readFromFile(char *file);
};

#endif