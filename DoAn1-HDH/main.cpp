#include "FAT32.h"
#include <iostream>
#include "NTFS.h"
using namespace std;

int main() {
	char letter;
	cout << "Enter drive letter: "; cin >> letter;

	FAT32 fat32(letter);
	fat32.readInfo();

	if (fat32.getOEMName() == "NTFS") {
		NTFS ntfs(letter);
		ntfs.readInfo();
		while (true) {
			system("cls");
			cout << " Volume " << (char)toupper(letter) << " - NTFS" << endl << endl;
			cout << " [1] View the information" << endl;
			cout << " [2] Display directory tree information" << endl;
			cout << endl;
			cout << " [0] Exit";
			cout << endl << endl;
			int choice;
			cout << " Enter your choice: "; cin >> choice;
			switch (choice)
			{
			case 0:
				return 0;
			case 1:
				ntfs.printInfo();
				system("pause");
				break;
			case 2:
				ntfs.displayDirectory();
				break;
			default:
				break;
			}
		}
	}
	else 
	{
		while (true) {
			system("cls");
			cout << " Volume " << (char)toupper(letter) << " - FAT32" << endl << endl;
			cout << " [1] View the information" << endl;
			cout << " [2] Display directory tree information" << endl;
			cout << endl;
			cout << " [0] Exit";
			cout << endl << endl;
			int choice;
			cout << " Enter your choice: "; cin >> choice;
			switch (choice)
			{
			case 0:
				return 0;
			case 1:
				fat32.printInfo();
				system("pause");
				break;
			case 2:
				fat32.displayDirectory();
				break;
			default:
				break;
			}
		}
	}

	return 0;
}