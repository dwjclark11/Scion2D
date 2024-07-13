#include "Rendering/Essentials/TextureLoader.h"
#include <SOIL/SOIL.h>
#include <Logger/Logger.h>

namespace SCION_RENDERING
{

bool TextureLoader::LoadTexture( const std::string& filepath, GLuint& id, int& width, int& height, bool blended )
{
	int channels = 0;

	unsigned char* image = SOIL_load_image( filepath.c_str(), // Filename			-- Image file to be loaded
											&width,			  // Width			-- Width of the image
											&height,		  // height			-- Height of the image
											&channels,		  // channels			-- Number of channels
											SOIL_LOAD_AUTO	  // force_channels	-- Force the channels count
	);

	// Check to see if the image is successful
	if ( !image )
	{
		SCION_ERROR( "SOIL failed to load image [{0}] -- {1}", filepath, SOIL_last_result() );
		return false;
	}

	GLint format = GL_RGBA;

	switch ( channels )
	{
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	if ( !blended )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	}
	else
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}

	glTexImage2D( GL_TEXTURE_2D,	// target			-- Specifies the target texture
				  0,				// level			-- Level of detail. 0 is the base image level
				  format,			// internal format	-- The number of color components
				  width,			// width			-- width of the texture image
				  height,			// height			-- height of the texture image
				  0,				// border
				  format,			// format			-- format of the pixel data
				  GL_UNSIGNED_BYTE, // type				-- The data type of the pixel data
				  image				// data
	);

	// Delete the image data from SOIL
	SOIL_free_image_data( image );

	return true;
}

bool TextureLoader::LoadFBTexture( GLuint& id, int& width, int& height )
{
	glBindTexture( GL_TEXTURE_2D, id );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	return true;
}

bool TextureLoader::LoadTextureFromMemory( const unsigned char* imageData, size_t length, GLuint& id, int& width,
										   int& height, bool blended )
{
	id = SOIL_load_OGL_texture_from_memory( imageData, length, SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, NULL );

	if ( id == 0 )
	{
		SCION_ERROR( "Failed to load texture from memory." );
		return false;
	}

	// Get the width and height data from the texture
	glBindTexture( GL_TEXTURE_2D, id );
	glad_glGetTextureLevelParameteriv( id, 0, GL_TEXTURE_WIDTH, &width );
	glad_glGetTextureLevelParameteriv( id, 0, GL_TEXTURE_HEIGHT, &height);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	if ( !blended )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	}
	else
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}

	return true;
}

std::shared_ptr<Texture> TextureLoader::Create( Texture::TextureType type, const std::string& texturePath,
												bool bTileset )
{
	GLuint id;
	int width, height;

	glGenTextures( 1, &id );
	glBindTexture( GL_TEXTURE_2D, id );

	switch ( type )
	{
	case Texture::TextureType::PIXEL: LoadTexture( texturePath, id, width, height, false ); break;
	case Texture::TextureType::BLENDED: LoadTexture( texturePath, id, width, height, true ); break;
	// TODO: Add other texture types for loading as needed -- Ex Framebuffer texture
	default: assert( false && "The current type is not defined, Please use a defined texture type!" ); return nullptr;
	}

	return std::make_shared<Texture>( id, width, height, type, texturePath, bTileset );
}

std::shared_ptr<Texture> TextureLoader::Create( Texture::TextureType type, int width, int height, bool bTileset )
{
	SCION_ASSERT( type == Texture::TextureType::FRAMEBUFFER && "Must be framebuffer type" );

	if ( type != Texture::TextureType::FRAMEBUFFER )
	{
		SCION_ERROR( "Failed to create texture for frame buffer. Incorrect type passed in!" );
		return nullptr;
	}

	GLuint id;
	glGenTextures( 1, &id );
	LoadFBTexture( id, width, height );

	return std::make_shared<Texture>( id, width, height, type, "", bTileset );
}

std::shared_ptr<Texture> TextureLoader::CreateFromMemory( const unsigned char* imageData, size_t length, bool blended,
														  bool bTileset )
{
	GLuint id;
	int width, height;

	LoadTextureFromMemory( imageData, length, id, width, height, blended );

	return std::make_shared<Texture>(
		id, width, height, blended ? Texture::TextureType::BLENDED : Texture::TextureType::PIXEL, "", bTileset );
}
} // namespace SCION_RENDERING