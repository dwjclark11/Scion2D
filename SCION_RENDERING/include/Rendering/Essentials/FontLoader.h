#pragma once
#include <memory>
#include <string>

namespace Scion::Rendering
{
class FontLoader
{
  public:
	FontLoader() = delete;

	/*
	 * @brief Creates a shared ptr to a Font class based on the passed in params.
	 * @param A string for the font path, a float for the font's size, and the width and height of the generated font
	 * texture.
	 * @return Returns a shared_ptr to a font class if successful, nullptr otherwise.
	 */
	static std::shared_ptr<class Font> Create( const std::string& fontPath, float fontSize = 32.f, int width = 512,
											   int height = 512 );

	/*
	 * @brief Creates a shared ptr to a Font class based on the passed in params.
	 * @param A const unsigned char* for the fontData. This is data that has been converted from a ttf file to a char
	 * array.
	 * @param A float for the font's size, and the width and height of the generated font
	 * texture.
	 * @return Returns a shared_ptr to a font class if successful, nullptr otherwise.
	 */
	static std::shared_ptr<class Font> CreateFromMemory( const unsigned char* fontData, float fontSize = 32.f,
														 int width = 512, int height = 512 );
};
} // namespace Scion::Rendering
