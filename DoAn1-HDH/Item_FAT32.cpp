#include "Item_FAT32.h"
#include "Utils.h"
#include <iostream>

Item_FAT32::Item_FAT32() {}

void Item_FAT32::printInfo(int number) {
	cout << " [" << number << "] ";
	if (isFile()) cout << "File: ";
	else cout << "Folder: ";
	cout << getFullname() << endl;
	cout << "     Attributes: ";
	if (isReadOnly()) cout << "read-only ";
	if (isHidden()) cout << "hidden ";
	if (isSystem()) cout << "system ";
	cout << endl;
	if (isFile()) {
		cout << "     Size: " << Utils::formatSize(size) << endl;
	}
	cout << "     First sector: " << sectors[0] << endl;
}

void Item_FAT32::setName(string name) {	this->name = name; }
string Item_FAT32::getName() { return name; }
void Item_FAT32::setExtension(string ext) { this->extension = ext; }
string Item_FAT32::getExtension() { return extension; }

string Item_FAT32::getFullname() {
	if (isFile() && !extension.empty()) {
		return name + '.' + extension;
	}
	return name;
}

bool Item_FAT32::isTXT() {
	string ext = extension;
	for (int i = 0; i < ext.size(); i++) {
		ext[i] = toupper(ext[i]);
	}
	return ext == "TXT";
}

void Item_FAT32::setAttribute(BYTE attribute) { this->attribute = attribute; }
BYTE Item_FAT32::getAttribute() { return attribute; }
bool Item_FAT32::isReadOnly() { return Utils::getBit(attribute, ATTR_BIT_READ_ONLY); }
bool Item_FAT32::isHidden() { return Utils::getBit(attribute, ATTR_BIT_HIDDEN); }
bool Item_FAT32::isSystem() { return Utils::getBit(attribute, ATTR_BIT_SYSTEM); }
bool Item_FAT32::isFile() { return Utils::getBit(attribute, ATTR_BIT_ARCHIVE); }
bool Item_FAT32::isFolder() { return Utils::getBit(attribute, ATTR_BIT_DIRECTORY); }

void Item_FAT32::setSize(long long size) { this->size = size; }
long long Item_FAT32::getSize() { return size; }

void Item_FAT32::setFirstSector(int firstCluster) { this->firstCluster = firstCluster; }
int Item_FAT32::getFirstSector() { return firstCluster; }

void Item_FAT32::setSectors(vector<int> sectors) { this->sectors = sectors; }
vector<int> Item_FAT32::getSectors() { return sectors; }

void Item_FAT32::setChildren(vector<Item_FAT32> children) { this->children = children; }
vector<Item_FAT32> Item_FAT32::getChildren() { return children; }

