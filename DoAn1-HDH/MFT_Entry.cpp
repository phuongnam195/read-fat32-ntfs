#include "MFT_Entry.h"
#include <iostream>
#include <vector>
#include "Utils.h"

MFT_Entry::MFT_Entry() {
	_firstSector = -1;
	_size = 0;
}

unsigned int MFT_Entry::getId() {
	return _id;
}

unsigned int MFT_Entry::getParentId() {
	return _parentId;
}

string MFT_Entry::getName() {
	return _name;
}

string MFT_Entry::getData() {
	return _data;
}

unsigned long long int MFT_Entry::getSize() {
	return _size;
}

int MFT_Entry::getAttributes() {
	return _attributes;
}

bool MFT_Entry::isReadOnly() {
	return Utils::getBit(_attributes, PERM_READ_ONLY);
}

bool MFT_Entry::isHidden() {
	return Utils::getBit(_attributes, PERM_HIDDEN);
}

bool MFT_Entry::isSystem() {
	return Utils::getBit(_attributes, PERM_SYSTEM);
}

bool MFT_Entry::isFile() {
	return _isFile;
}

bool MFT_Entry::isFolder() {
	return !_isFile;
}

int MFT_Entry::getFirstSector() {
	return _firstSector;
}

void MFT_Entry::setId(unsigned int id) {
	_id = id;
}

void MFT_Entry::setParentId(unsigned int parentId) {
	_parentId = parentId;
}

void MFT_Entry::setName(string name) {
	_name = name;
}

void MFT_Entry::setData(string data) {
	_data = data;
}
void MFT_Entry::setSize(unsigned long long int  size) {
	_size = size;
}

void MFT_Entry::setAttributes(int attributes) {
	_attributes = attributes;
}

void MFT_Entry::setIsFile(bool isFile) {
	_isFile = isFile;
}

void MFT_Entry::setFirstSector(int firstSector) {
	_firstSector = firstSector;
}

void MFT_Entry::printInfo(int number) {
	cout << " [" << number << "] ";
	if (_isFile) cout << "File: ";
	else cout << "Folder: ";
	cout << _name << endl;
	
	cout << "     Attributes: ";
	if (isReadOnly()) cout << "read-only ";
	if (isHidden()) cout << "hidden ";
	if (isSystem()) cout << "system ";
	cout << endl;

	if (_isFile) {
		cout << "     Size: " << Utils::formatSize(_size) << endl;
	}
	cout << "     Data: ";
	if (_firstSector == -1) {
		cout << "resident" << endl;
	}
	else {
		cout << "non-resident (sector " << _firstSector << ")" << endl;
	}
}

bool MFT_Entry::isTXT() {
	if (!_isFile) {
		return false;
	}

	string extension;
	vector<string> fileName = Utils::split(_name, ".");
	string temp = fileName[1];
	for (int i = 0; i < temp.size(); i++) { // Delete " " in string
		if (temp[i] != '\0')
			extension += temp[i];
	}
	for (int i = 0; i < extension.size(); i++) {
		extension[i] = toupper(extension[i]);
	}
	return extension == "TXT";
}
