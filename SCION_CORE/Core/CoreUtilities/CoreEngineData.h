#pragma once
#include <chrono>

namespace SCION_CORE {
	class CoreEngineData
	{
	private:
		float m_DeltaTime, m_ScaledWidth, m_ScaledHeight;
		std::chrono::steady_clock::time_point m_LastUpdate;
		int m_WindowWidth, m_WindowHeight;
		int32_t m_VelocityIterations, m_PositionIterations;

		bool m_bPhysicsEnabled, m_bPhysicsPaused, m_bRenderColliders;

	private:
		CoreEngineData();
		~CoreEngineData() = default;
		CoreEngineData(const CoreEngineData&) = delete;
		CoreEngineData& operator=(const CoreEngineData&) = delete;

	public:
		static CoreEngineData& GetInstance();
		void UpdateDeltaTime();
		void SetWindowWidth(int windowWidth);
		void SetWindowHeight(int windowHeight);

		const float MetersToPixels() const;
		const float PixelsToMeters() const;

		inline const float GetDeltaTime() const { return m_DeltaTime; }
		inline const int WindowWidth() const { return m_WindowWidth; }
		inline const int WindowHeight() const { return m_WindowHeight; }
		
		inline void EnableColliderRender() { m_bRenderColliders = true; }
		inline void DisableColliderRender() { m_bRenderColliders = false; }
		inline const bool RenderCollidersEnabled() { return m_bRenderColliders; }

		inline const float ScaledWidth() const { return m_ScaledWidth; }
		inline const float ScaledHeight() const { return m_ScaledHeight; }

		inline const int32_t GetVelocityIterations() const { return m_VelocityIterations; }
		inline const int32_t GetPositionIterations() const { return m_PositionIterations; }

		inline void EnablePhysics() { m_bPhysicsEnabled = true; }
		inline void DisablePhysics() { m_bPhysicsEnabled = false; }
		inline void PausePhysics() { m_bPhysicsPaused = true; }
		inline void UnPausePhysics() { m_bPhysicsPaused = false; }
		inline const bool IsPhysicsEnabled() const { return m_bPhysicsEnabled; }
		inline const bool IsPhysicsPaused() const { return m_bPhysicsPaused; }
	};
}