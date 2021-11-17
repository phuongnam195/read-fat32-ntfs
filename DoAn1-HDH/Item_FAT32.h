#pragma once

#include <Windows.h>
#include <string>
#include <vector>
using namespace std;

#define ATTR_BIT_READ_ONLY		0
#define ATTR_BIT_HIDDEN			1
#define ATTR_BIT_SYSTEM			2
#define ATTR_BIT_VOLUME_ID		3
#define ATTR_BIT_DIRECTORY		4
#define ATTR_BIT_ARCHIVE		5

#define FAT_FREE			0x00000000
#define FAT_BAD				0xFFFFFFF7
#define FAT_EOF1			0xFFFFFFF8
#define FAT_EOF2			0xFFFFFFFF
#define FAT_EOF3			0x0FFFFFFF

// Entry chính
struct MainEntry {
	BYTE Name[8];			// 0 File name, 8
	BYTE Ext[3];			// 8 Extension, 3
	BYTE Attr;				// 11 File attributes. (00ARSHDV)
	BYTE None[8];			// None
	BYTE FstClusLH[2];		// 20 First cluster number, high word.
	BYTE WrtTime[2];		// 22 Last write time.
	BYTE WrtDate[2];		// 24 Last write date.
	BYTE FstClusLO[2];		// 26 First cluster number, low word.
	BYTE FileSize[4];		// 28 File size in bytes.
};

// Entry phụ
struct ExtraEntry {
	BYTE STT;				// số thứ tự
	BYTE Name1[10];			//các kí tự tên (5 kí tự)
	BYTE Attr;				//(0x0F)
	BYTE Resverved;			//(0x00)
	BYTE CheckSum;			//check sum của file
	BYTE Name2[12];			//Các kí tự tên file (6 kí tự)
	BYTE FirstCluser[2];	//Cluster đầu tiên
	BYTE Name3[4];			//Các kí tự tên file (2 kí tự)
};

// Một Item_FAT32 có thể là file hoặc folder (cũng có thể là root: ổ đĩa)
class Item_FAT32 {
private:
	string name;					// Tên file hoặc folder
	string extension;				// Tên mở rộng (chỉ file mới có)
	BYTE attribute;					// Thuộc tính
	long long size;					// Kích thước của file
	int firstCluster;				// Chỉ số sector lưu trữ
	vector<int> sectors; 			// Mảng sector
	vector<Item_FAT32> children;	// Các item con, trong trường hợp đây là folder
public:
	Item_FAT32();

	void printInfo(int number);		// In thông tin của 1 file/folder
	
	void setName(string name);
	string getName();
	void setExtension(string ext);
	string getExtension();
	string getFullname();			// Lấy tên (ghép đuôi file nếu có)
	bool isTXT();					// true nếu đây là file .txt

	void setAttribute(BYTE attribute);
	BYTE getAttribute();
	bool isReadOnly();
	bool isHidden();
	bool isSystem();
	bool isFile();
	bool isFolder();

	void setSize(long long size);
	long long getSize();

	void setFirstSector(int firstCluster);
	int getFirstSector();

	void setSectors(vector<int> sectors);
	vector<int> getSectors();

	void setChildren(vector<Item_FAT32> children);
	vector<Item_FAT32> getChildren();
};