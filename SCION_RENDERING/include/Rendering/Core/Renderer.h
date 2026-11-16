#pragma once
#include <glad/glad.h>
#include "Rendering/Essentials/Primitives.h"
#include <memory>
#include <vector>

namespace sol
{
class state;
}

namespace Scion::Rendering
{

class ShapeRenderer;
class SpriteBatchRenderer;
class TextBatchRenderer;
class PostProcessPipeline;
class Framebuffer;

class Renderer
{
  public:
	enum class GLCapability : GLuint
	{
		BLEND = GL_BLEND,
		DEPTH_TEST = GL_DEPTH_TEST,
		LINE_SMOOTH = GL_LINE_SMOOTH,
	};

	enum class BlendingFactors : GLuint
	{
		ONE = GL_ONE,
		SRC_ALPHA = GL_SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
	};

  public:
	Renderer();
	~Renderer();

	// OpenGL Render function wrappers
	/*
	 * @brief Sets the color buffers to the screen when the buffers are cleared.
	 * @brief Values are clamped at [0, 1]
	 * @param takes in GLfloats for all colors RGBA
	 */
	void SetClearColor( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
	void ClearBuffers( bool color = true, bool depth = true, bool stencil = false );

	void SetCapability( GLCapability capability, bool enable );
	bool IsCapabilityEnabled( GLCapability capability ) const;

	void SetBlendCapability( BlendingFactors sFactor, BlendingFactors dFactor );
	void SetViewport( GLint x, GLint y, GLsizei width, GLsizei height );

	void SetLineWidth( GLfloat lineWidth );

	// Primitive Draw calls
	void DrawLine( const Line& line );
	void DrawLine( const glm::vec2& p1, const glm::vec2& p2, const Color& color, float lineWidth = 1.f );

	void DrawRect( const Rect& rect );
	void DrawRect( const glm::vec2& position, float width, float height, const Color& color );
	void DrawFilledRect( const Rect& rect );

	void DrawCircle( const Circle& circle );
	void DrawCircle( const glm::vec2& position, float radius, const Color& color, float thickness = 1.f );

	void DrawText2D( const Text& text );

	// Draws every queued line, rect and circle with one shader and one batch
	void DrawShapes( class Shader& shader, class Camera2D& camera );
	void DrawAllText( class Shader& shader, class Camera2D& camera );

	void ClearPrimitives();

	void CreateFBO( const std::string& name, int width, int height, bool bUseRBO = false );
	void BindFBO( const std::string& name );
	void UnbindFBO();

	void DeleteFBO( const std::string& name );
	bool HasFBO( const std::string& name ) const;
	void BindTextureUnit( GLuint unit, GLuint textureID );

	[[nodiscard]] GLuint GetFBTextureID( const std::string& name );
	[[nodiscard]] GLuint GetActiveFBTextureID() const;
	[[nodiscard]] Framebuffer* GetFramebuffer( const std::string& name );
	void DrawScreen( GLuint textureID );
	void OnViewportSizeChange( float width, float height );

	static void CreateLuaBind( sol::state& lua, Renderer& renderer );

  private:
	void CreateScreenData();
	void ClearScreen( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, bool color = true, bool depth = true );

  private:
	std::vector<Text> m_Text;

	std::unique_ptr<ShapeRenderer> m_pShapeBatch;
	std::unique_ptr<SpriteBatchRenderer> m_pSpriteBatch;
	std::unique_ptr<TextBatchRenderer> m_pTextBatch;

	std::unordered_map<std::string, std::unique_ptr<Framebuffer>> m_mapFramebuffers;

	GLuint m_ScreenVAO;
	GLuint m_ScreenVBO;
	GLuint m_ScreenEBO;
};
} // namespace Scion::Rendering
