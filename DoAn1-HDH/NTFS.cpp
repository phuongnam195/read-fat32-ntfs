#include "NTFS.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
using namespace std;

#define FRAGMENTED_LIMIT 50

NTFS::NTFS() {}

NTFS::NTFS(char letter) {
	this->driveLetter = toupper(letter);
}

NTFS::~NTFS() {}

void NTFS::readInfo() {
	BYTE result[512];
	Utils::ReadSector(Utils::getStrLetter(driveLetter), 0, result);
	memcpy(&info, result, 512);

	bytesPerSector = Utils::reverseByte(info.Bytes_Sector, 2);
	sectorsPerCluster = info.Sectors_Cluster;
	sizeOfMFTEntry = (int)pow(2, abs(info.Cluster_FRS));
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
	cout << "| Size of MFT entry        | " << setw(SPACE) << sizeOfMFTEntry << " |" << endl;
	cout << "| Cluster per Index Buffer | " << setw(SPACE) << (int)info.Cluster_Index_Buffer << " |" << endl;
	cout << "|__________________________|_____________________|" << endl;
	cout << endl;
}

void NTFS::displayDirectory() {
	scanAllEntries();

	vector<int> stkDirIdx;			// stack directory index

	while (true) {
		system("cls");

		cout << " " << driveLetter << ":\\";
		for (int i = 0; i < stkDirIdx.size(); i++) {
			cout << mftEntries[stkDirIdx[i]].getName() << "\\";
		}

		vector<int> childIndices;
		int parentID = ROOT_INDEX;
		if (!stkDirIdx.empty()) {
			parentID = mftEntries[stkDirIdx[stkDirIdx.size() - 1]].getId();
		}
		cout << endl;

		for (int i = 0; i < mftEntries.size(); i++) {
			if (mftEntries[i].getParentId() == parentID) {
				childIndices.push_back(i);
				cout << "  " << BRANCH << " " << mftEntries[i].getName() << endl;
			}
		}
		cout << endl << endl;

		cout << " [0] Back" << endl << endl;
		for (int i = 0; i < childIndices.size(); i++) {
			mftEntries[childIndices[i]].printInfo(i + 1);
			cout << endl;
		}

		int choice;
		do {
			cout << "Enter your choice: "; cin >> choice;
		} while (choice < 0 || choice > childIndices.size());
		choice--;
		if (choice == -1) {
			if (stkDirIdx.size() > 0) {
				stkDirIdx.pop_back();
			}
			else {
				return;
			}
		}
		else {
			int index = childIndices[choice];
			if (mftEntries[index].isFolder()) {
				stkDirIdx.push_back(index);
			}
			else {
				printData(mftEntries[index]);
				cout << endl;
				system("pause");
			}
		}
	}
}

void NTFS::scanAllEntries() {
	mftEntries.clear();

	int offset = info.Logical_MFT * sectorsPerCluster * bytesPerSector;
	int emptyEntriesCount = 0;
	BYTE* bytes = new BYTE[sizeOfMFTEntry];

	vector<unsigned int> systemFolderIDs;		// Chứa ID các thư mục system

	// Nếu số entry rỗng liên tiếp vượt FRAGMENTED_LIMIT (50) thì ngừng đọc
	while (emptyEntriesCount < FRAGMENTED_LIMIT) {
		Utils::ReadSector(Utils::getStrLetter(driveLetter), offset, bytes, sizeOfMFTEntry);
		offset += sizeOfMFTEntry;
		if (Utils::reverseByte(bytes, 4) == 0) {
			emptyEntriesCount++;
			continue;
		}

		emptyEntriesCount = 0;

		int flag = Utils::reverseByte(bytes + 22, 2);

		// Not in use (đã xóa)
		if (Utils::getBit(flag, 0) == 0) {
			continue;
		}

		MFT_Entry entry = readEntry(bytes);
		entry.setIsFile(!Utils::getBit(flag, 1));

		if (entry.isSystem()
			|| count(systemFolderIDs.begin(), systemFolderIDs.end(), entry.getParentId())) {
			if (entry.isFolder() && entry.getId() != ROOT_INDEX) {
				systemFolderIDs.push_back(entry.getId());
			}
		}
		else {
			mftEntries.push_back(entry);
		}
	}

	delete[] bytes;
}

MFT_Entry NTFS::readEntry(BYTE bytes[]) {
	MFT_Entry result;

	unsigned int id = Utils::reverseByte(bytes + 44, 4);
	result.setId(id);

	int offsetAttribute = Utils::reverseByte(bytes + 20, 2);
	while (true) {
		if (Utils::reverseByte(bytes + offsetAttribute, 4) == 0xFFFFFFFF) {
			break;
		}

		int attributeType = Utils::reverseByte(bytes + offsetAttribute, 2);
		int offsetContent = offsetAttribute + Utils::reverseByte(bytes + offsetAttribute + 20, 2);
		if (attributeType == ATTR_STANDARD_INFORMATION) {
			long long permission = Utils::reverseByte(bytes + offsetContent + 32, 4);
			result.setAttributes(permission);
		}
		else if (attributeType == ATTR_FILE_NAME) {
			unsigned int parentId = Utils::reverseByte(bytes + offsetContent, 2);
			result.setParentId(parentId);

			long long size = Utils::reverseByte(bytes + offsetContent + 48, 8);
			result.setSize(size);

			string name;
			int nameLength = Utils::reverseByte(bytes + offsetContent + 64, 1);
			for (int i = 0; i < nameLength; i++) {
				name.push_back(bytes[offsetContent + 66 + i * 2]);
			}
			result.setName(name);
		}
		else if (attributeType == ATTR_DATA) {
			bool nonResidentFlag = Utils::reverseByte(bytes + offsetContent + 8, 1);
			if (nonResidentFlag) {
				int firstCluster = Utils::reverseByte(bytes + offsetContent + 2, 3);
				result.setFirstSector(firstCluster * sectorsPerCluster);
			}
			else {
				string data;
				for (int i = 0; i < result.getSize(); i++) {
					data.push_back(bytes[offsetContent + i]);
				}
				result.setData(data);
			}
		}
		offsetAttribute += Utils::reverseByte(bytes + offsetAttribute + 4, 4);
	}

	return result;
}

void NTFS::printData(MFT_Entry file) {
	if (!file.isTXT()) {
		cout << "> Please use compatible software to read the content!";
		return;
	}

	if (file.getSize() == 0) {
		return;
	}

	if (file.getFirstSector() == -1) {
		cout << file.getData();
	}
	else {
		long long offset = file.getFirstSector() * bytesPerSector;
		long long size = file.getSize();
		BYTE* data = new BYTE[size + 1];

		long long readSize = ((size / 512) + 1) * 512;
		Utils::ReadSector(Utils::getStrLetter(driveLetter), offset, data, readSize);
		data[size] = '\0';
		cout << data;

		delete[] data;
	}

}