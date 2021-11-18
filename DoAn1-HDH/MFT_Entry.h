#pragma once

#include <string>
using namespace std;

#define PERM_READ_ONLY              0
#define PERM_HIDDEN                 1
#define PERM_SYSTEM                 2

class MFT_Entry {
private:
	unsigned int _id;
	unsigned int _parentId;
	string _name;
	string _data;
	long long _size;
	int _attributes;
	bool _isFile;
	long _firstSector;			// Sector bắt đầu của dữ liệu, dùng trong trường hợp non-resident
public:
	MFT_Entry();

	unsigned int getId();
	unsigned int getParentId();
	string getName();
	string getData();
	long long getSize();
	int getAttributes();
	bool isReadOnly();
	bool isHidden();
	bool isSystem();
	bool isFile();
	bool isFolder();
	int getFirstSector();

	void setId(unsigned int);
	void setParentId(unsigned int);
	void setName(string);
	void setData(string);
	void setSize(long long);
	void setAttributes(int);
	void setIsFile(bool);
	void setFirstSector(int);

	void printInfo(int number);
	bool isTXT();
};