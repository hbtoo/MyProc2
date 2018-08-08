#include <iostream>
#include <string>
#include <math.h>
#include <fstream>
using namespace std;


long reverseNum(long input, bool encode) {
	if (encode) {
		input = 2 * input + 1;
	}
	long output = 0;
	long i;
	while (input > 0) {
		i = input % 10;
		input = input - i;
		input = input / 10;
		output = output * 10 + i;
	}
	if (!encode) {
		output = (output - 1) / 2;
	}
	return output;
}

void reverseChar(char *input) {
	char tmp;
	int i, j, len = strlen(input);
	for (i = 0; i < len; i++) {
		j = len - 1 - i;
		if (i >= j)
			break;
		tmp = input[i];
		input[i] = input[j];
		input[j] = tmp;
	}
}

char* convertTo36(long input) {
	char* result = new char [100];
	int len = 0;
	long i;
	while (input > 0) {
		i = input % 36;
		input = input - i;
		input = input / 36;
		if (i < 10)
			result[len] = (char)('0' + i);
		else if (i == 24)	// Use lower case o instead of upper to avoid confusion with zero
			result[len] = (char)('a' + i - 10);
		else
			result[len] = (char)('A' + i - 10);
		len++;
	}
	result[len] = 0;
	reverseChar(result);
	return result;
}

char* encode(long input) {
	input = reverseNum(input, true);
	return convertTo36(input);
}

long decode(char *input) {
	long output = 0;
	int i;
	for (i = 0; i < strlen(input); i++) {
		output = output * 36;
		if (input[i] >= '0' && input[i] <= '9') {
			output = output + (input[i] - '0');
		} else if (input[i] >= 'a' && input[i] <= 'z') {
			output = output + (input[i] - 'a') + 10;
		} else if (input[i] >= 'A' && input[i] <= 'Z') {
			output = output + (input[i] - 'A') + 10;
		} else
			return 0;
	}
	return reverseNum(output, false);
}

long encryptNum(long n) {
	return (n + 101) ^ 714;
}

long decryptNum(long n) {
	return (n ^ 714) -  101;
}

bool convertFile(char *input, char *output, bool encrypt) {
	int i, j;
	FILE *f = fopen(input, "r");
	if (f == NULL) {
		cout << "找不到输入文件 " << input << endl;
		return false;
	}
	fclose(f);

	f = fopen(output, "r");
	if (f != NULL) {
		cout << "输出文件已存在 " << output << endl;
		return false;
	}

	ifstream fin(input);
	if (!fin.is_open()) {
		cout << "找不到输入文件 " << input << endl;
		return false;
	}
	ofstream fout(output);
	if (!fout.is_open()) {
		cout << "不能创建输出文件 " << output << endl;
		return false;
	}

	bool firstOutput = true;
	while (!fin.eof()) {
		fin >> i;
		if (encrypt) {
			j = encryptNum(i);
		} else {
			j = decryptNum(i);
		}

		if (firstOutput) {
			firstOutput = false;
		} else {
			fout << endl;
		}
		fout << j;
	}

	fin.close();
	fout.close();
	cout << "文件转换成功" << endl;
	return true;
}

void convertToLetter(int mode) {
	char str[300];
	long i;
	printf("开始输入，输入0表示结束\n");
	while (true) {
		scanf("%s", str, sizeof(str));
		if (strcmp(str, "0") == 0)
			break;

		if (mode == 1) {
			i = 0;
			sscanf(str, "%ld", &i);
			
			char* result = encode(i);
			printf(" ---> %s\n", result);
			delete result;
		} else {
			printf(" ---> %ld\n", decode(str));
		}
	}
}

void main()
{
	char str[300];
	cout << "选择 1: 数字 -> 字母代号\n" 
		 << "     2: 字母代号 -> 数字\n"
		 << "     3: D:/input.txt 加密到 D:/encripted.txt \n"
		 << "     4: D:/encripted.txt 解密到 D:/decripted.txt \n";
	scanf("%s", str, sizeof(str));
	if (strcmp(str, "1") == 0) {
		convertToLetter(1);
		return;
	} else if (strcmp(str, "2") == 0) {
		convertToLetter(2);
		return;
	} else if (strcmp(str, "3") == 0) {
		convertFile("D:/input.txt", "D:/encripted.txt", true);
	} else if (strcmp(str, "4") == 0) {
		convertFile("D:/encripted.txt", "D:/decripted.txt", false);
	} else {
		cout << "不支持选择" << str << endl;
	}
	char c = getchar();
	cout << "按回车退出" << endl;
	c = getchar();
}
