#pragma once
#include <chrono>
#include <unordered_map>

#define CORE_GLOBALS() SCION_CORE::CoreEngineData::GetInstance()

namespace SCION_CORE
{

enum class EGameType
{
	TopDown,
	Platformer,
	Fighting,
	Puzzle,
	Rougelike,

	NoType
};

class CoreEngineData
{
  public:
	static CoreEngineData& GetInstance();
	void UpdateDeltaTime();
	void SetWindowWidth( int windowWidth );
	void SetWindowHeight( int windowHeight );

	const float MetersToPixels() const;
	const float PixelsToMeters() const;

	void SetScaledWidth( float newWidth );
	void SetScaledHeight( float newHeight );

	inline double GetDeltaTime() const { return m_DeltaTime; }
	inline int WindowWidth() const { return m_WindowWidth; }
	inline int WindowHeight() const { return m_WindowHeight; }

	inline void EnableColliderRender() { m_bRenderColliders = true; }
	inline void DisableColliderRender() { m_bRenderColliders = false; }
	inline bool RenderCollidersEnabled() const { return m_bRenderColliders; }

	inline float ScaledWidth() const { return m_ScaledWidth; }
	inline float ScaledHeight() const { return m_ScaledHeight; }

	inline int32_t GetVelocityIterations() const { return m_VelocityIterations; }
	inline int32_t GetPositionIterations() const { return m_PositionIterations; }
	inline void SetVelocityIterations( int32_t velocityIterations ) { m_VelocityIterations = velocityIterations; }
	inline void SetPositionIterations( int32_t positionIterations ) { m_PositionIterations = positionIterations; }
	inline float GetGravity() const { return m_Gravity; }
	inline void SetGravity( float gravity ) { m_Gravity = gravity; }
	inline void EnablePhysics() { m_bPhysicsEnabled = true; }
	inline void DisablePhysics() { m_bPhysicsEnabled = false; }
	inline void PausePhysics() { m_bPhysicsPaused = true; }
	inline void UnPausePhysics() { m_bPhysicsPaused = false; }
	inline const bool IsPhysicsEnabled() const { return m_bPhysicsEnabled; }
	inline const bool IsPhysicsPaused() const { return m_bPhysicsPaused; }

	inline const std::string& GetProjectPath() const { return m_sProjectPath; }
	inline void SetProjectPath( const std::string& sPath ) { m_sProjectPath = sPath; }

	inline void SetGameType( EGameType eType ) { m_eGameType = eType; }
	inline EGameType GetGameType() const { return m_eGameType; }

	static std::string GetGameTypeStr( EGameType eType );
	static EGameType GetGameTypeFromStr( const std::string& sType );

	static const std::unordered_map<EGameType, std::string>& GetGameTypesMap();
	static void RegisterMetaFunctions();

  private:
	CoreEngineData();
	~CoreEngineData() = default;
	CoreEngineData( const CoreEngineData& ) = delete;
	CoreEngineData& operator=( const CoreEngineData& ) = delete;

  private:
	double m_DeltaTime;
	float m_ScaledWidth;
	float m_ScaledHeight;
	float m_Gravity;
	std::chrono::steady_clock::time_point m_LastUpdate;
	int m_WindowWidth;
	int m_WindowHeight;
	int32_t m_VelocityIterations;
	int32_t m_PositionIterations;

	bool m_bPhysicsEnabled;
	bool m_bPhysicsPaused;
	bool m_bRenderColliders;
	std::string m_sProjectPath;

	EGameType m_eGameType{ EGameType::NoType };
};
} // namespace SCION_CORE
