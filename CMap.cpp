
#include "stdafx.h"
#include "CMap.h"
void Cmap::getlevelData(char* filename)
{
	//ifstream filePtr;
	//fstream filePtr(filename, ios::out);
	//
	string str;
	////filePtr.open(filename, ios::out);//�}���ɮ�a:\textIO.txt
	//	while(filePtr>>str)    //Ū�@�Ӥ@�Ӧr���str��
	//	{mapStatus.push_back(str);}     
	ifstream filePtr;
	filePtr.open(filename, ios::out);//�}���ɮ�a:\textIO.txt
	while(filePtr>>str)    //Ū�@�Ӥ@�Ӧr���str��
		{mapStatus.push_back(atoi(str.c_str()));}    
	xsize =  mapStatus.at(0); 
	zsize = mapStatus.at(1);
	return;
}