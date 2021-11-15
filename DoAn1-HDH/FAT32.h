#pragma once

#include "Item_FAT32.h"
#include <cstdint>
#include <vector>
#include <string>
using namespace std;

struct Bootsector {
	uint8_t BS_jmpBoot[3];                      //Lệnh nhảy đến đoạn boot code - 3 bytes
	uint8_t BS_OEMName[8];                      //Version / tên hệ điều hành - 8 bytes
	uint8_t BPB_BytsPerSec[2];                  //Số bytes/sector - 2 bytes
	uint8_t BPB_SecPerClus[1];                  //Số sector/cluster - 1 bytes
	uint8_t BPB_RsvdSecCnt[2];                  //Số sector để dành(khác 0) (Số sector trước bảng Fat) - 2 bytes
	uint8_t BPB_NumFATs[1];                     //Số bảng Fat - 1 bytes
	uint8_t BPB_RootEntCnt[2];                  //  - 2 bytes
	uint8_t TotSec16[2];                        //  - 2 bytes
	uint8_t BPB_Media[1];                       //Loại volume - 1 bytes

	uint8_t BPB_FATSz16[2];                     // - 2 bytes
	uint8_t BPB_SecPerTrk[2];                   //Số sector/track - 2 bytes
	uint8_t BPB_NumHeads[2];                    //Số heads - 2 bytes
	uint8_t BPB_HiddSec[4];                     //Số sector ẩn trước Volume - 4 bytes
	uint8_t BPB_TotSec32[4];                    //Số sector trong Volume - 4 bytes
	uint8_t BPB_FATSz32[4];                     //Số sector trong 1 bảng FAT - 4 bytes
	uint8_t BPB_ExtFlags[2];                    // - 2 bytes
	uint8_t BPB_FSVer[2];                       //Version FAT32 - 2 bytes
	uint8_t BPB_RootClus[4];                    //Chỉ số cluster đầu tiên RDET - 4 bytes
	uint8_t BPB_FSInfo[2];                      //Chỉ số sector chứa FSINFO - thông tin sector trống - 2 bytes
	uint8_t BPB_BkBootSec[2];                   //Chỉ số sector chứa bản sao của bootsector - 2 bytes
	uint8_t BPB_Reserved[12];                   //Dành riêng - 12 bytes
	uint8_t BS_DrvNum[1];                       //Kí hiệu vật lí đĩa - 1 bytes
	uint8_t BS_Reservedl[1];                    //Dành riêng - 1 bytes
	uint8_t BS_BootSig[1];                      //Kí hiệu nhận diện hệ điều hành - 1 bytes
	uint8_t BS_VolID[4];                        //Volume serial numbers - 4 bytes
	uint8_t BS_VolLab[11];                      //Volume label - 11 bytes
	uint8_t BS_FilSysType[8];                   //Chuỗi nhận diện loại FAT - 8 bytes
	uint8_t BootCode[420];                      //Boot Code - 420 bytes
	uint8_t EndBootCode[2];                     // End Boot Code - 2 bytes
};

class FAT32 {
private:
	char driveLetter;				// Kí tự ô đĩa
	Bootsector info;				// 512 bytes của Bootsector
	int bytesPerSector;				// Số byte của 1 sector
	int sectorsPerCluster;			// Số sector của 1 cluster
	uint64_t sectorFAT1;			// Sector đầu tiên của FAT1
	uint64_t sectorRDET;			// Sector đầu tiên của RDET
	string volumeName;				// Tên ổ đĩa
	vector<unsigned long> FAT1;		// Bảng FAT1 gồm các long (4 bytes)

public:
	FAT32();
	FAT32(char letter);
	~FAT32();

	void readInfo();				// Đọc bootsector
	void printInfo();				// In bảng bootsector

	uint64_t getOffsetRDET();
	uint64_t getBytesPerSector();

	void readFAT1();				// Đọc bảng FAT1

	void displayDirectory();									// Xem và truy cập cây thư mục
	vector<Item_FAT32> scanItemsInRDET();						// Quét file/folder của thư mục gốc
	vector<Item_FAT32> scanItemsInSDET(vector<int> sectors);	// Quét file/folder của thư mục con, với tham số là mảng các sector bắt đầu

	void printData(Item_FAT32 file);	// In dữ liệu của 1 file

	string getOEMName();				// Mục đích: Nhận biết FAT32 hay NTFS
};