#pragma once
#include "Batcher.h"
#include "Rendering/Essentials/BatchTypes.h"

namespace Scion::Rendering
{

class CircleBatchRenderer : public Batcher<RectBatch, CircleGlyph>
{
  public:
	CircleBatchRenderer();
	virtual ~CircleBatchRenderer() = default;

	virtual void End() override;
	virtual void Render() override;
	void AddCircle( const glm::vec4& destRect, const Color& color, float thickness,
					glm::mat4 model = glm::mat4{ 1.0f } );
	void AddCircle( const struct Circle& circle );

  private:
	virtual void GenerateBatches() override;
	void Initialize();
};
} // namespace Scion::Rendering
