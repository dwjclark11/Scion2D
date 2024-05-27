#pragma once
#include "Batcher.h"
#include "Rendering/Essentials/BatchTypes.h"

namespace SCION_RENDERING {
	class SpriteBatchRenderer : public Batcher<Batch, SpriteGlyph>
	{
	private: // Functions
		void Initialize();
		virtual void GenerateBatches() override;

	public:
		SpriteBatchRenderer();
		~SpriteBatchRenderer() = default;

		/*
		* @brief Checks to see if there are sprites to create batches.
		* Sorts the sprites based on their layer and then generates the
		* batches to be rendered.
		*/
		virtual void End() override;

		/*
		* @brief Checks to see if there are any batches to render. If
		* there are batches to render, it loops through the batches and Renders them.
		*/
		virtual void Render() override;
		
		/*
		* @brief Adds a new sprite to the sprites vector.
		* @param glm::vec4 spriteRect is the transform position of the sprite quad.
		* @param glm::vec4 uvRect is the UVs that the current sprite is using for its texture.
		* @param GLuint textureID is the OpenGL texture ID
		* @param glm::mat4 model is the model matrix to apply transformations to the sprites verticies
		* @param Color is the color the sprite is changed to.
		*/
		void AddSprite(const glm::vec4& spriteRect, const glm::vec4 uvRect, GLuint textureID, int layer = 0
			, glm::mat4 model = glm::mat4{ 1.f }, const Color& color = Color{ .r = 255, .g = 255, .b = 255, .a = 255 });
	};
}