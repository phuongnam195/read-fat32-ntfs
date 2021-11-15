#include "NTFS.h"
#include "Utils.h"
#include <Windows.h>
#include <iostream>
#include <iomanip>

NTFS::NTFS() {}

NTFS::NTFS(char letter) {
	this->driveLetter = letter;
}

NTFS::~NTFS() {}

void NTFS::readInfo() {
	BYTE result[512];
	Utils::ReadSector(Utils::getStrLetter(driveLetter), 0, result);
	memcpy(&info, result, 512);

	bytesPerSector = Utils::reverseByte(info.Bytes_Sector, 2);
	sectorsPerCluster = info.Sectors_Cluster;
}

void NTFS::printInfo() {
	const int SPACE = 19;
	system("cls");
	cout << " ________________________________________________" << endl;
	cout << "|                       VBR                      |" << endl;
	cout << "|------------------------------------------------|" << endl;
	cout << "| Bytes per sector         | " << setw(SPACE) << bytesPerSector << " |" << endl;
	cout << "| Sectors per cluster      | " << setw(SPACE) << sectorsPerCluster << " |" << endl;
	cout << "| Media descriptor         | " << setw(SPACE) << (info.Media_Descriptor == 0xF8 ? "Fixed Disk (F8)" : Utils::decToHex(info.Media_Descriptor)) << " |" << endl;
	cout << "| Sectors per track        | " << setw(SPACE) << Utils::reverseByte(info.Sectors_Track, 2) << " |" << endl;
	cout << "| Number of heads          | " << setw(SPACE) << Utils::reverseByte(info.number_of_Heads, 2) << " |" << endl;
	cout << "| Hidden sectors           | " << setw(SPACE) << Utils::reverseByte(info.Hidden_sectors, 4) << " |" << endl;
	cout << "| Total sectors            | " << setw(SPACE) << info.total_sectors << " |" << endl;
	cout << "| $MFT cluster number      | " << setw(SPACE) << info.Logical_MFT << " |" << endl;
	cout << "| $MFTMirr cluster number  | " << setw(SPACE) << info.Logical_MFTMirr << " |" << endl;
	cout << "| Size of MFT entry        | " << setw(SPACE) << (int)pow(2, abs(info.Cluster_FRS)) << " |" << endl;
	cout << "| Cluster per Index Buffer | " << setw(SPACE) << (int)info.Cluster_Index_Buffer << " |" << endl;
	cout << "| Volume serial number:    | " << setw(SPACE) << info.Volume << " |" << endl;
	cout << "|__________________________|_____________________|" << endl;
	cout << endl;
}
