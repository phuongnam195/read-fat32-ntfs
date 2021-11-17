#pragma once
#include "MFT_Entry.h"
#include <cstdint>
#include <Windows.h>
#include <vector>

#define ATTR_STANDARD_INFORMATION   0x10
#define ATTR_FILE_NAME              0x30
#define ATTR_DATA                   0x80

#define ROOT_INDEX                  5

struct VBR {
    uint8_t jumpCode[3];            // Lệnh nhảy đến đoạn boot code - 3 bytes
    uint8_t OEM_ID[8];              // Loại - 8 bytes
    uint8_t Bytes_Sector[2];        // Kích thước một sector(tính bằng byte) - 2 bytes
    uint8_t Sectors_Cluster;        // Sectors / Cluster: Số sector trên cluster - 1 bytes
    uint8_t Reserved_Sector[2];     //Số sector dự trữ - 2 bytes
    uint8_t always0_1[3];           // luôn có giá trị là 0 - 3 bytes
    uint8_t not_used_by_NTFS_1[2];  // Ko được sử dụng bởi NTFS - 2 bytes
    uint8_t Media_Descriptor;       // Mã xác định loại đĩa - 1 bytes
    uint8_t always0_2[2];           // luôn có giá trị là 0 - 2 bytes
    uint8_t Sectors_Track[2];       // Số sector trên một track - 2 bytes
    uint8_t number_of_Heads[2];     // Số đầu đọc (heads) - 2 bytes
    uint8_t Hidden_sectors[4];      // Số sector ẩn - 4 bytes
    uint8_t not_used_by_NTFS_2[4];  // Ko được sử dụng bởi NTFS - 4 bytes
    uint8_t not_used_by_NTFS_3[4];  // Ko được sử dụng bởi NTFS - 4 bytes
    long long total_sectors;        // Tổng số sector trên phân vùng - 8 bytes
    long long Logical_MFT;          // Cluster bắt đầu của $MFT - 8 bytes
    long long Logical_MFTMirr;      // Cluster bắt đầu của $MFTMirr (bản sao của $MFT) - 8 bytes
    int8_t Cluster_FRS;             // Số cluster trên một phân đoạn bản ghi file - 4 bytes
    uint8_t not_used_by_NTFS_4[3];  // Ko được sử dụng bởi NTFS - 3 bytes
    int8_t Cluster_Index_Buffer;    // Số cluster trên một Index Buffer - 1 bytes
    uint8_t not_used_by_NTFS_5[3];  // Ko được sử dụng bởi NTFS - 3 bytes
    long long Volume;               // Volume Serial Number - 8 bytes
    uint8_t checksum[4];            // Checksum - 4 bytes
    uint8_t Bootstrap_Code[426];    // Đoạn chương trình được nạp khi khởi động - 426 bytes
    uint8_t EndofSectorMarker[2];   // Dấu hiệu kết thúc - 2 bytes
};

class NTFS {
private:
    char driveLetter;               // Kí tự ô đĩa
    VBR info;                       // 512 bytes của VBR
    int bytesPerSector;				// Số byte của 1 sector
    int sectorsPerCluster;			// Số sector của 1 cluster
    int sizeOfMFTEntry;             // Kích thước của 1 MFT entry
    vector<MFT_Entry> mftEntries;   // Danh sách các MFT entry

public:
    NTFS();
    NTFS(char letter);
    ~NTFS();

    void readInfo();				// Đọc VBR
    void printInfo();				// In bảng VBR

    void displayDirectory();
    void scanAllEntries();
    MFT_Entry readEntry(BYTE bytes[]);
    
    void printData(MFT_Entry file);
};