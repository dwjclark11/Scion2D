#pragma once
#include <cstdint>

namespace Scion::Rendering
{
#pragma pack( push, 1 )

struct ICONDIR
{
	uint16_t reserved; // Always 0
	uint16_t type;	   // 1 for icons
	uint16_t count;	   // Number of images
};

struct ICONDIRENTRY
{
	uint8_t width;	// 0 = 256
	uint8_t height; // 0 = 256
	uint8_t colorCount;
	uint8_t reserved;
	uint16_t planes;
	uint16_t bitCount;
	uint32_t bytesInRes;
	uint32_t imageOffset;
};

struct BITMAPINFOHEADER
{
	uint32_t size;
	int32_t width;
	int32_t height; // note: includes mask height (XOR + AND)
	uint16_t planes;
	uint16_t bitCount;
	uint32_t compression;
	uint32_t sizeImage;
	int32_t xPelsPerMeter;
	int32_t yPelsPerMeter;
	uint32_t clrUsed;
	uint32_t clrImportant;
};

#pragma pack( pop )
}
