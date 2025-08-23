#pragma once
#include "AbstractTool.h"
#include "Core/ECS/Entity.h"

namespace SCION_RENDERING
{
class SpriteBatchRenderer;
}

namespace SCION_EDITOR
{
class TileTool : public AbstractTool
{
  private:
	glm::vec2 m_MouseRect;
	bool m_bGridSnap;

  protected:
	std::shared_ptr<SCION_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;
	std::shared_ptr<struct Tile> m_pMouseTile;

  protected:
	uint32_t CheckForTile( const glm::vec2& position );

	SCION_CORE::ECS::Entity CreateEntity();
	SCION_CORE::ECS::Entity CreateEntity( uint32_t id );

	void DrawMouseSprite();
	virtual void ExamineMousePosition() override;

  public:
	TileTool();
	virtual ~TileTool() = default;

	virtual void Create() = 0;
	virtual void Draw() = 0;

	virtual void Update( SCION_CORE::Canvas& canvas ) override;

	/*
	 * @brief Clears the Mouse Tile's sprite component values back to default.
	 */
	void ClearMouseTextureData();

	/*
	 * @brief Sets the moust tile's sprite component to the passed in texture name.
	 * Also generates UVs based on that texture. The texture must exist in the Asset Manager
	 * for this function to work successfully.
	 * @param Takes in a string for the texture name.
	 */
	void LoadSpriteTextureData( const std::string& textureName );

	/*
	 * @brief Get the current texture name assigned to the sprite component of the mouse tile.
	 */
	const std::string& GetSpriteTexture() const;

	/*
	 * @brief Sets the startX and startY values of the mouse tile's sprite component, then it
	 * will recalculate the uv values.
	 * @param int values for the startX and startY of the sprite.
	 */
	void SetSpriteUVs( int startX, int startY );

	/*
	 * @brief Sets the mouse rect and sprite width/height for the mouse tile's sprite component.
	 * This will also check to see if the sprite is valid and recalculate the uv values.
	 * @param Takes in a glm::vec2 for the sprite rect which should be the width and height.
	 */
	void SetSpriteRect( const glm::vec2& spriteRect );

	/*
	 * @brief Checks to see if the sprite texture name has been set.
	 * @return Returns true if the sprite texture name is not empty, false otherwise.
	 */
	const bool SpriteValid() const;

	/*
	 * @brief Checks to see if the tile can be drawn. Checks if over tilemap window, if the mouse it out of bounds,
	 * If the sprite is valid, and if the current layer is set for the selected scene.
	 */
	const bool CanDrawOrCreate() const;

	inline void EnableGridSnap() { m_bGridSnap = true; }
	inline void DisableGridSnap() { m_bGridSnap = false; }
	inline const bool IsGridSnapEnabled() const { return m_bGridSnap; }

	inline Tile& GetTileData() { return *m_pMouseTile; }
};
} // namespace SCION_EDITOR
