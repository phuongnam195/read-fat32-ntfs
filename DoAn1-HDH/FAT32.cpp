#include "FAT32.h"
#include "Utils.h"
#include "Item_FAT32.h"
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stack>
using namespace std;

FAT32::FAT32() {}

FAT32::FAT32(char letter) {
	this->driveLetter = toupper(letter);
}

FAT32::~FAT32() {}

void FAT32::readInfo() {
	BYTE result[512];
	Utils::ReadSector(Utils::getStrLetter(driveLetter), 0, result);
	memcpy(&info, result, 512);

	bytesPerSector = Utils::reverseByte(info.BPB_BytsPerSec, 2);
	sectorsPerCluster = Utils::reverseByte(info.BPB_SecPerClus, 1);
	sectorFAT1 = Utils::reverseByte(info.BPB_RsvdSecCnt, 2);
	sectorRDET = sectorFAT1 + Utils::reverseByte(info.BPB_NumFATs, 1) * Utils::reverseByte(info.BPB_FATSz32, 4);
}

void FAT32::printInfo() {
	const int SPACE = 18;
	system("cls");
	cout << " ________________________________________________" << endl;
	cout << "|                   BOOTSECTOR                   |" << endl;
	cout << "|------------------------------------------------|" << endl;
	cout << "| Bytes per sector          | " << setw(SPACE) << bytesPerSector << " |" << endl;
	cout << "| Sectors per cluster       | " << setw(SPACE) << sectorsPerCluster << " |" << endl;
	cout << "| Reserved sectors          | " << setw(SPACE) << Utils::reverseByte(info.BPB_RsvdSecCnt, 2) << " |" << endl;
	cout << "| Number of FATs            | " << setw(SPACE) << Utils::reverseByte(info.BPB_NumFATs, 1) << " |" << endl;
	cout << "| Media descriptor          | " << setw(SPACE) << (Utils::reverseByte(info.BPB_Media,1) == 0xF8 ? "Fixed Disk (F8)" : Utils::decToHex(Utils::reverseByte(info.BPB_Media, 1))) << " |" << endl;
	cout << "| Sectors per track         | " << setw(SPACE) << Utils::reverseByte(info.BPB_SecPerTrk, 2) << " |" << endl;
	cout << "| Number of Heads           | " << setw(SPACE) << Utils::reverseByte(info.BPB_NumHeads, 2) << " |" << endl;
	cout << "| Hidden sectors            | " << setw(SPACE) << Utils::reverseByte(info.BPB_HiddSec, 4) << " |" << endl;
	cout << "| Total sectors             | " << setw(SPACE) << Utils::reverseByte(info.BPB_TotSec32, 4) << " |" << endl;
	cout << "| Sectors per FAT           | " << setw(SPACE) << Utils::reverseByte(info.BPB_FATSz32, 4) << " |" << endl;
	cout << "| RDET cluster              | " << setw(SPACE) << Utils::reverseByte(info.BPB_RootClus, 4) << " |" << endl;
	cout << "| Backup Boot sector        | " << setw(SPACE) << Utils::reverseByte(info.BPB_BkBootSec, 2) << " |" << endl;
	string logicalDriveNumber;
	if (Utils::reverseByte(info.BS_DrvNum, 1) == 0x00) {
		logicalDriveNumber = "Floppy Disk (00)";
	}
	else if (Utils::reverseByte(info.BS_DrvNum, 1) == 0x80) {
		logicalDriveNumber = "Hard Disk (80)";
	}
	else {
		logicalDriveNumber = "0x" + Utils::decToHex(Utils::reverseByte(info.BS_DrvNum, 1));
	}
	cout << "| Physical drive            | " << setw(SPACE) << logicalDriveNumber << " |" << endl;
	cout << "| First sector of FAT1      | " << setw(SPACE) << sectorFAT1 << " |" << endl;
	cout << "| First sector of RDET      | " << setw(SPACE) << sectorRDET << " |" << endl;
	cout << "|___________________________|____________________|" << endl;
	cout << endl;
}

uint64_t FAT32::getOffsetRDET() {
	return sectorRDET * bytesPerSector;
}

uint64_t FAT32::getBytesPerSector() {
	return bytesPerSector;
}

void FAT32::readFAT1() {
	uint64_t sectorsPerFAT = Utils::reverseByte(info.BPB_FATSz32, 4);
	uint64_t bytesPerFAT = sectorsPerFAT * bytesPerSector;
	BYTE* bytesFAT1 = new BYTE[bytesPerFAT];
	Utils::ReadSector(Utils::getStrLetter(driveLetter), sectorFAT1 * bytesPerSector, bytesFAT1, bytesPerFAT);
	for (int i = 0; i < bytesPerFAT / 4; i++) {
		long value = bytesFAT1[i * 4 + 3];
		value = value * 16 * 16 + bytesFAT1[i * 4 + 2];
		value = value * 16 * 16 + bytesFAT1[i * 4 + 1];
		value = value * 16 * 16 + bytesFAT1[i * 4];
		FAT1.push_back(value);
	}
	delete[] bytesFAT1;
}

void FAT32::printData(Item_FAT32 file) {
	if (!file.isTXT()) {
		cout << "> Please use compatible software to read the content!";
		return;
	}
	vector<int> sectors = file.getSectors();
	long long remainSize = file.getSize();
	for (int iSector = 0; iSector < sectors.size(); iSector++) {
		long long offset = sectors[iSector]; offset *= 512;
		BYTE data[513];
		Utils::ReadSector(Utils::getStrLetter(driveLetter), offset, data);
		data[512] = '\0';
		if (remainSize >= 512) {
			cout << data;
			remainSize -= 512;
		}
		else {
			for (int i = 0; i < remainSize; i++) {
				cout << data[i];
			}
			return;
		}
	}
}

void FAT32::displayDirectory() {
	vector<Item_FAT32> stkDir;		// stack directory
	vector<int> stkIdx;				// stack index	
	Item_FAT32 root;
	root.setChildren(scanItemsInRDET());
	stkDir.push_back(root);

	while (true) {
		system("cls");

		if (stkDir.size() == 1) {
			cout << " " << volumeName << " (" << driveLetter << ":)" << endl;
		}
		else {
			cout << " " << driveLetter << ":\\";
			for (int i = 1; i < stkDir.size(); i++) {
				cout << stkDir[i].getName() << "\\";
			}
			cout << endl;
		}

		vector<Item_FAT32> children = stkDir[stkDir.size() - 1].getChildren();
		for (int i = 0; i < children.size(); i++) {
			cout << "  " << BRANCH << " " << children[i].getFullname() << endl;
		}
		cout << endl << endl;

		cout << " [0] Back" << endl << endl;
		for (int i = 0; i < children.size(); i++) {
			children[i].printInfo(i + 1);
			cout << endl;
		}

		int choice;
		do {
			cout << "Enter your choice: "; cin >> choice;
			
		} while (choice < 0 || choice > children.size());
		choice--;
		if (choice == -1) {
			if (stkDir.size() > 1) {
				stkDir.pop_back();
			}
			else {
				return;
			}
		}
		else {
			if (children[choice].isFolder()) {
				children[choice].setChildren(scanItemsInSDET(children[choice].getSectors()));
				stkDir.push_back(children[choice]);
			}
			else {
				printData(children[choice]);
				cout << endl;
				system("pause");
			}
		}
	}
}

vector<Item_FAT32> FAT32::scanItemsInRDET() {
	vector<Item_FAT32> result;

	stack<ExtraEntry> stkExtraEntry;

	if (FAT1.empty()) {
		readFAT1();
	}

	BYTE bytesRDET[4096];
	Utils::ReadSector(Utils::getStrLetter(driveLetter), sectorRDET * bytesPerSector, bytesRDET, 4096);
	int index = 0;

	while (index < 4096 / 32) {
		if ((int)bytesRDET[index * 32] == 0xE5) {
			index++;
			continue;
		}
		BYTE entry[32];
		memcpy(&entry, bytesRDET + index * 32, 32);

		MainEntry mainEntry;
		memcpy(&mainEntry, entry, 32);
		index++;

		if (mainEntry.Attr == 0x0F) {
			ExtraEntry extraEntry;
			memcpy(&extraEntry, entry, 32);
			stkExtraEntry.push(extraEntry);
		}
		else if (Utils::getBit(mainEntry.Attr, ATTR_BIT_SYSTEM)) {
			while (!stkExtraEntry.empty()) {
				stkExtraEntry.pop();
			}
			continue;
		}
		else if (Utils::getBit(mainEntry.Attr, ATTR_BIT_VOLUME_ID)) {
			stringstream builder;
			if (stkExtraEntry.empty()) {
				for (int i = 0; i < 8; i++) {
					builder << mainEntry.Name[i];
				}
			}
			else {
				while (!stkExtraEntry.empty()) {
					ExtraEntry extraEntry = stkExtraEntry.top(); stkExtraEntry.pop();
					for (int i = 0; i < 5; i++) {
						builder << extraEntry.Name1[i * 2];
					}
					for (int i = 0; i < 6; i++) {
						builder << extraEntry.Name2[i * 2];
					}
					for (int i = 0; i < 2; i++) {
						builder << extraEntry.Name3[i * 2];
					}
				}
			}

			volumeName = Utils::trimRight(builder.str());
		}
		else if (Utils::getBit(mainEntry.Attr, ATTR_BIT_ARCHIVE) ||
			Utils::getBit(mainEntry.Attr, ATTR_BIT_DIRECTORY)) {
			Item_FAT32 file;
			stringstream builder;

			if (stkExtraEntry.empty()) {
				for (int i = 0; i < 8; i++) {
					builder << mainEntry.Name[i];
				}
				file.setName(Utils::trimRight(builder.str()));

				builder.str("");
				for (int i = 0; i < 3; i++) {
					builder << mainEntry.Ext[i];
				}
				file.setExtension(Utils::trimRight(builder.str()));
			}
			else {
				while (!stkExtraEntry.empty()) {
					ExtraEntry extraEntry = stkExtraEntry.top(); stkExtraEntry.pop();
					for (int i = 0; i < 5; i++) {
						builder << extraEntry.Name1[i * 2];
					}
					for (int i = 0; i < 6; i++) {
						builder << extraEntry.Name2[i * 2];
					}
					for (int i = 0; i < 2; i++) {
						builder << extraEntry.Name3[i * 2];
					}
					string rawName = builder.str();
					if (rawName.find('.') == string::npos) {
						file.setName(Utils::trimRight(rawName));
					}
					else {
						int dot = rawName.size() - 1;
						while (rawName[dot] != '.') dot--;
						file.setName(Utils::trimRight(rawName.substr(0, dot)));
						file.setExtension(Utils::trimRight(rawName.substr(dot + 1)));
					}
				}
			}
			file.setAttribute(mainEntry.Attr);
			file.setSize(Utils::reverseByte(mainEntry.FileSize, 4));
			int firstCluster = Utils::reverseByte(mainEntry.FstClusLH, 2) * 10 + Utils::reverseByte(mainEntry.FstClusLO, 2);
			file.setFirstSector(firstCluster);
			if (firstCluster != 0) {
				int clt = firstCluster;
				vector<int> sectors;
				while (clt != FAT_EOF1 && clt != FAT_EOF2 && clt != FAT_EOF3) {
					for (int i = 0; i < sectorsPerCluster; i++) {
						sectors.push_back(i + (clt - 2) * sectorsPerCluster + sectorRDET);
					}
					clt = FAT1[clt];
				}
				file.setSectors(sectors);
			}
			result.push_back(file);
		}
	}

	return result;
}

vector<Item_FAT32> FAT32::scanItemsInSDET(vector<int> sectors) {
	vector<Item_FAT32> result;

	stack<ExtraEntry> stkExtraEntry;

	if (FAT1.empty()) {
		readFAT1();
	}

	for (int iSector = 0; iSector < sectors.size(); iSector++) {
		BYTE bytesSector[512];
		Utils::ReadSector(Utils::getStrLetter(driveLetter), (long long)sectors[iSector] * bytesPerSector, bytesSector);
		
		for (int iEntry = 0; iEntry < 512 / 32; iEntry++) {
			BYTE entry[32];
			memcpy(&entry, bytesSector + iEntry * 32, 32);

			if ((int)entry[0] == 0xE5 || (int)entry[0] == 0x2E) {
				continue;
			}

			MainEntry mainEntry;
			memcpy(&mainEntry, entry, 32);

			if (mainEntry.Attr == 0x0F) {
				ExtraEntry extraEntry;
				memcpy(&extraEntry, entry, 32);
				stkExtraEntry.push(extraEntry);
			} 
			else if (Utils::getBit(mainEntry.Attr, ATTR_BIT_ARCHIVE) ||
				Utils::getBit(mainEntry.Attr, ATTR_BIT_DIRECTORY)) {
				Item_FAT32 file;
				stringstream builder;

				if (stkExtraEntry.empty()) {
					for (int i = 0; i < 8; i++) {
						builder << mainEntry.Name[i];
					}
					file.setName(Utils::trimRight(builder.str()));

					builder.str("");
					for (int i = 0; i < 3; i++) {
						builder << mainEntry.Ext[i];
					}
					file.setExtension(Utils::trimRight(builder.str()));
				}
				else {
					while (!stkExtraEntry.empty()) {
						ExtraEntry extraEntry = stkExtraEntry.top(); stkExtraEntry.pop();
						for (int i = 0; i < 5; i++) {
							builder << extraEntry.Name1[i * 2];
						}
						for (int i = 0; i < 6; i++) {
							builder << extraEntry.Name2[i * 2];
						}
						for (int i = 0; i < 2; i++) {
							builder << extraEntry.Name3[i * 2];
						}
						string rawName = builder.str();
						if (rawName.find('.') == string::npos) {
							file.setName(Utils::trimRight(rawName));
						}
						else {
							int dot = rawName.size() - 1;
							while (rawName[dot] != '.') dot--;
							file.setName(Utils::trimRight(rawName.substr(0, dot)));
							file.setExtension(Utils::trimRight(rawName.substr(dot + 1)));
						}
					}
				}
				file.setAttribute(mainEntry.Attr);
				file.setSize(Utils::reverseByte(mainEntry.FileSize, 4));
				int firstCluster = Utils::reverseByte(mainEntry.FstClusLH, 2) * 10 + Utils::reverseByte(mainEntry.FstClusLO, 2);
				file.setFirstSector(firstCluster);
				if (firstCluster != 0) {
					int clt = firstCluster;
					vector<int> sectors;
					while (clt != FAT_EOF1 && clt != FAT_EOF2 && clt != FAT_EOF3) {
						for (int i = 0; i < sectorsPerCluster; i++) {
							sectors.push_back((clt - 2) * sectorsPerCluster + sectorRDET);
						}
						clt = FAT1[clt];
					}
					file.setSectors(sectors);
				}
				result.push_back(file);
			}
		}
	}

	return result;
}

string FAT32::getOEMName() {
	stringstream builder;
	for (int i = 0; i < 8; i++) {
		builder << (char)info.BS_OEMName[i];
	}
	return Utils::trimRight(builder.str());
}