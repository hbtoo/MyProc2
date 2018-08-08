
#include "stdafx.h"
#include "data.h"
#include <fstream.h>

long decrypt(long n) {
	return (n ^ 714) -  101;
}

bool Data::readFromFile(char *file) {
	int i, j;
	int tmp;

	FILE *f = fopen(file, "r");
	CString c;
	c.Format("找不到输入文件%s", file);
	if (f == NULL) {
		AfxMessageBox(c);
		return false;
	}
	fclose(f);

	ifstream fin(file);
	if (!fin.is_open()) {
		AfxMessageBox(c);
		return false;
	}

	c.Format("文件前两个数必需是非负数");
	fin >> tmp;
	nInput = decrypt(tmp);

	fin >> tmp;
	nOutput = decrypt(tmp);
	if (nInput <= 0 || nOutput <= 0) {
		AfxMessageBox(c);
		return false;
	}

	inArray = new int [nInput];
	for (i = 0; i < nInput; i++) {
		if (fin.eof()) {
			c.Format("文件格式错误");
			AfxMessageBox(c);
			return false;
		}
		fin >> tmp;
		inArray[i] = decrypt(tmp);
		if (inArray[i] < 0) {
			if (fin.eof())
				c.Format("文件格式错误");
			AfxMessageBox(c);
			return false;
		}
	}

	outArray = new int *[nInput];
	for (i = 0; i < nInput; i++) {
		outArray[i] = new int [nOutput];
		for (j = 0; j < nOutput; j++) {
			if (fin.eof()) {
				c.Format("文件格式错误");
				AfxMessageBox(c);
				return false;
			}
			fin >> tmp;
			outArray[i][j] = decrypt(tmp);
			if (outArray[i][j] < 0) {
				if (fin.eof())
					c.Format("文件格式错误");
				AfxMessageBox(c);
				return false;
			}
		}
	}
	fin.close();
	return true;
}
