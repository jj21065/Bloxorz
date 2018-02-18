#include <vector>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
using namespace std;
class Cmap
{
public:
	int xsize,zsize;
	vector<int> mapStatus;
	COLORREF goalColor,floorColor;
	void getlevelData(char* filename);
};