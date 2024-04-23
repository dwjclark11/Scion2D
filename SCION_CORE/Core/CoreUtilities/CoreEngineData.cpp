#include "CoreEngineData.h"
#include <Logger/Logger.h>

namespace SCION_CORE {
	constexpr float METERS_TO_PIXELS = 12.f;
	constexpr float PIXELS_TO_METERS = 1.f / METERS_TO_PIXELS;

	CoreEngineData::CoreEngineData()
		: m_DeltaTime{ 0.f }, m_ScaledWidth{ 0.f }, m_ScaledHeight{ 0.f }
		, m_WindowWidth{ 640 }, m_WindowHeight{ 480 }
		, m_VelocityIterations{ 10 }, m_PositionIterations{ 8 }
		, m_bPhysicsEnabled{ true }, m_bPhysicsPaused{ false }, m_bRenderColliders{ false }
	{
		m_ScaledWidth = m_WindowWidth / METERS_TO_PIXELS;
		m_ScaledHeight = m_WindowHeight / METERS_TO_PIXELS;
	}

	CoreEngineData& CoreEngineData::GetInstance()
	{
		static CoreEngineData instance{};
		return instance;
	}

	void CoreEngineData::UpdateDeltaTime()
	{
		auto now = std::chrono::steady_clock::now();
		m_DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - m_LastUpdate).count() / 1000000.0f;
		m_LastUpdate = now;
		// TODO: Fix Delta Time
	}

	void CoreEngineData::SetWindowWidth(int windowWidth)
	{
		m_WindowWidth = windowWidth;
		m_ScaledWidth = m_WindowWidth / METERS_TO_PIXELS;
	}

	void CoreEngineData::SetWindowHeight(int windowHeight)
	{
		m_WindowHeight = windowHeight;
		m_ScaledHeight = m_WindowHeight / METERS_TO_PIXELS;
	}

	const float CoreEngineData::MetersToPixels() const
	{
		return METERS_TO_PIXELS;
	}

	const float CoreEngineData::PixelsToMeters() const
	{
		return PIXELS_TO_METERS;
	}

}