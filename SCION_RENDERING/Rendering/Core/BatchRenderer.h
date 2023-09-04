#pragma once
#include "../Essentials/Vertex.h"
#include <vector>
#include <memory>

namespace SCION_RENDERING {
	class BatchRenderer
	{
	private: // Member structs
		struct Batch
		{
			GLuint numIndices{ 0 }, offset{ 0 }, textureID{ 0 };
		};

		struct Sprite
		{
			Vertex topLeft, bottomLeft, topRight, bottomRight;
			int layer;
			GLuint textureID;
		};

	private: // Class Members
		GLuint m_VAO, m_VBO, m_IBO;
		std::vector<std::shared_ptr<Sprite>> m_Sprites;
		std::vector<std::shared_ptr<Batch>> m_Batches;

	private: // Functions
		void Initialize();
		void GenerateBatches();

	public:
		BatchRenderer();
		~BatchRenderer();

		/*
		* @brief Clears the current batches and sprites making
		* it ready to start new batches.
		*/
		void Begin();

		/*
		* @brief Checks to see if there are sprites to create batches.
		* Sorts the sprites based on their layer and then generates the
		* batches to be rendered.
		*/
		void End();

		/*
		* @brief Checks to see if there are any batches to render. If
		* there are batches to render, it loops through the batches and Renders them.
		*/
		void Render();
		
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