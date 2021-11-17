#pragma warning(disable:4996)

#include "Utils.h"
#include <sstream>
#include <sstream>
#include <iomanip>

bool Utils::ReadSector(LPCWSTR drive, UINT64 offset, BYTE* result, UINT32 length) {
	if (length % 512 != 0) {
		return false;
	}

	int retCode = 0;
	DWORD bytesRead;
	HANDLE device = NULL;

	device = CreateFile(drive,  // Drive to open
		GENERIC_READ,           // Access mode
		FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
		NULL,                   // Security Descriptor
		OPEN_EXISTING,          // How to create
		0,                      // File attributes
		NULL);                  // Handle to template

	// Open Error
	if (device == INVALID_HANDLE_VALUE) {
		return false;
	}

	LONG upper = offset >> 32;
	SetFilePointer(device, (UINT32)offset, &upper, FILE_BEGIN);	//Set a Point to Read

	bool success = ReadFile(device, result, length, &bytesRead, NULL);
	if (!success) {
		return false;
	}
	
	return true;
}

uint64_t Utils::reverseByte(uint8_t* byte, unsigned int count)
{
	uint64_t result = 0;
	int i;
	for (i = count - 1; i >= 0; i--)
		result = (result << 8) | byte[i];

	return result;
}

string Utils::decToHex(int d) {
	stringstream ss;
	ss << hex << d;
	return ss.str();
}

bool Utils::getBit(int n, int k) {
	return (n & (1 << k)) >> k;
}

string Utils::formatSize(long long size) {
	stringstream builder;
	if (size >= 1024*1024*1024) {
		double fSize = size;
		fSize /= (1024 * 1024 * 1024);
		builder << std::fixed << std::setprecision(2) << fSize << " GB";
	}
	else if (size >= 1024 * 1024) {
		double fSize = size;
		fSize /= (1024 * 1024);
		builder << std::fixed << std::setprecision(2) << fSize << " MB";
	}
	else if (size >= 1024) {
		double fSize = size;
		fSize /= 1024;
		builder << std::fixed << std::setprecision(2) << fSize << "KB";
	}
	else {
		builder << size << "B";
	}
	return builder.str();
}

string Utils::trimRight(string s) {
	while (s.size() > 0 && s[s.size() - 1] == ' ') {
		s.pop_back();
	}
	return s;
}

wchar_t* Utils::getStrLetter(char letter) {
	wchar_t* result;

	string s = "\\\\.\\";
	s.push_back(letter);
	s.push_back(':');
	const size_t cSize = strlen(s.c_str()) + 1;
	result = new wchar_t[cSize];
	mbstowcs(result, s.c_str(), cSize);

	return result;
}

vector<string> Utils::split(string haystack, string needle)
{
	vector<string> result;
	int startPos = 0;
	size_t foundPos = 0;

	while (true)
	{
		foundPos = haystack.find(needle, startPos);
		if (foundPos != string::npos) {
			string token = haystack.substr(startPos, foundPos - startPos);
			result.push_back(token);
			startPos = foundPos + needle.length();
		}
		else {
			string token = haystack.substr(startPos, haystack.length() - startPos);
			result.push_back(token);
			break;
		}
	}
	return result;
}