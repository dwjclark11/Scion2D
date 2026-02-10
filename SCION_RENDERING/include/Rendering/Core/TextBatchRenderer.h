#pragma once
#include "Batcher.h"
#include "Rendering/Essentials/BatchTypes.h"

namespace Scion::Rendering
{
class TextBatchRenderer : public Batcher<TextBatch, TextGlyph>
{
  public:
	TextBatchRenderer();
	virtual ~TextBatchRenderer() = default;

	virtual void End() override;
	virtual void Render() override;

	void AddText( const std::string& text, const std::shared_ptr<Font>& font, const glm::vec2& position,
				  int padding = 4, float wrap = 0.f, Color color = Color{ 255, 255, 255, 255 },
				  glm::mat4 model = glm::mat4{ 1.f } );

  private:
	void Initialize();
	virtual void GenerateBatches() override;
};
} // namespace Scion::Rendering
