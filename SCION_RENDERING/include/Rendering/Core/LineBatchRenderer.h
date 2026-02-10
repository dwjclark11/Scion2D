#pragma once
#include "Batcher.h"
#include "Rendering/Essentials/BatchTypes.h"
#include "Rendering/Essentials/Primitives.h"

namespace Scion::Rendering
{
class LineBatchRenderer : public Batcher<LineBatch, LineGlyph>
{
  public:
	LineBatchRenderer();
	virtual ~LineBatchRenderer() = default;

	virtual void End() override;
	virtual void Render() override;

	void AddLine( const Line& line );

  private:
	virtual void GenerateBatches() override;
	void Initialize();
};
} // namespace Scion::Rendering
