#pragma once

namespace SCION_WINDOWING::Inputs
{
struct Button
{
	bool bIsPressed{ false };
	bool bJustPressed{ false };
	bool bJustReleased{ false };

	void Update( bool bPressed );
	void Reset();
};
} // namespace SCION_WINDOWING::Inputs
