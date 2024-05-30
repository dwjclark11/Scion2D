#pragma once
#include "Batcher.h"
#include "Rendering/Essentials/BatchTypes.h"
#include "Rendering/Essentials/Primitives.h"

namespace SCION_RENDERING
{
class LineBatchRenderer : public Batcher<LineBatch, LineGlyph>
{
  private:
	virtual void GenerateBatches() override;
	void Initialize();

  public:
	LineBatchRenderer();
	~LineBatchRenderer() = default;

	virtual void End() override;
	virtual void Render() override;

	void AddLine(const Line& line);
};
} // namespace SCION_RENDERING