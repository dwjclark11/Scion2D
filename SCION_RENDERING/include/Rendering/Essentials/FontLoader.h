#pragma once

#include "Rendering/Essentials/Font.h"

#include <memory>
#include <span>
#include <string>

namespace Scion::Rendering
{

class FontLoader
{
  public:
	FontLoader() = delete;

	// Loads a font from disk and bakes printable ASCII characters [32, 127].
	// Requires an active OpenGL context. Returns nullptr on failure.
	[[nodiscard]] static std::unique_ptr<Font> Create( const std::string& fontPath, float fontSize = 32.f,
											   int width = 512, int height = 512 );

	// Preferred memory-based API: accepts a known-size, contiguous font buffer.
	// The buffer must contain a complete, valid TTF/OTF font.
	[[nodiscard]] static std::unique_ptr<Font> CreateFromMemory( std::span<const unsigned char> fontData,
														float fontSize = 32.f, int width = 512, int height = 512 );

	// Compatibility overload for existing callers with a raw font-data pointer.
	// Caller is responsible for keeping a complete font buffer valid during this call.
	[[nodiscard]] static std::unique_ptr<Font> CreateFromMemory( const unsigned char* fontData,
														float fontSize = 32.f, int width = 512, int height = 512 );
};

} // namespace Scion::Rendering
