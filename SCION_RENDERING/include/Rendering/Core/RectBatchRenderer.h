#pragma once
#include "Batcher.h"
#include "Rendering/Essentials/BatchTypes.h"

namespace SCION_RENDERING {

	class RectBatchRenderer : public Batcher<RectBatch, RectGlyph>
	{
	private:
		virtual void GenerateBatches() override;
		void Initialize();

	public:
		RectBatchRenderer();
		~RectBatchRenderer() = default;

		virtual void End() override;
		virtual void Render() override;

		void AddRect(const glm::vec4& destRect, int layer, const Color& color, glm::mat4 model = glm::mat4{ 1.f });
		void AddRect(const struct Rect& rect, glm::mat4 model = glm::mat4{1.f});
	};
}
