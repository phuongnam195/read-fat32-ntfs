#pragma once

#include <Windows.h>
#include <cstdint>
#include <string>
using namespace std;

class Utils {
public:
	// Đọc 1 khối 512x bytes bắt đầu từ offset vào mảng result
	static bool ReadSector(LPCWSTR  drive, UINT64 offset, BYTE* result, UINT32 length = 512);

	// Đổi một mảng byte có độ dài count về số thập phân
	static uint64_t reverseByte(uint8_t* byte, unsigned int count);

	// Chuyển số thập phân sang số thập lục phân (string)
	static string decToHex(int d);

	// Lấy giá trị bit thứ k của số n (đổi sang hệ nhị phân)
	static bool getBit(int n, int k);

	// Định dạng ngắn gọn cho kích thước: 3.5KB, 15.6MB
	static string formatSize(long long size);

	// Xóa các kí tự trắng bên phải xâu s
	static string trimRight(string s);

	// Chuyển letter thành string: \\\\.\\F:
	static wchar_t* getStrLetter(char letter);
};