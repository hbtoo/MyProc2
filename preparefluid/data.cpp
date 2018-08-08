
#include "stdafx.h"
#include "data.h"
#include <fstream.h>

bool Data::readFromFile(char *file) {
	int i, j;
	double tmp;

	FILE *f = fopen(file, "r");
	CString c;
	c.Format("�Ҳ��������ļ�%s", file);
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

	c.Format("�ļ�����ȫ���ǷǸ���");
	fin >> nInput >> nOutput;
	if (nInput <= 0 || nOutput <= 0) {
		AfxMessageBox(c);
		return false;
	}

	inArray = new double [nInput];
	for (i = 0; i < nInput; i++) {
		if (fin.eof()) {
			c.Format("�ļ���ʽ����");
			AfxMessageBox(c);
			return false;
		}
		fin >> inArray[i];
		if (inArray[i] < 0) {
			if (fin.eof())
				c.Format("�ļ���ʽ����");
			AfxMessageBox(c);
			return false;
		}
	}

	outArray = new int *[nInput];
	for (i = 0; i < nInput; i++) {
		outArray[i] = new int [nOutput];
		for (j = 0; j < nOutput; j++) {
			if (fin.eof()) {
				c.Format("�ļ���ʽ����");
				AfxMessageBox(c);
				return false;
			}
			fin >> tmp;
			if (tmp < 0) {
				if (fin.eof())
					c.Format("�ļ���ʽ����");
				AfxMessageBox(c);
				return false;
			}
			if (tmp != 0)
				outArray[i][j] = (int)tmp;	// change later !!!!!!!!!!!!!!!!!!!!!!!!!
			else
				outArray[i][j] = 0;
		}
	}
	fin.close();
	return true;
}
