#pragma once

namespace Scion::Windowing::Inputs
{
struct Button
{
	bool bIsPressed{ false };
	bool bJustPressed{ false };
	bool bJustReleased{ false };

	void Update( bool bPressed );
	void Reset();
};
} // namespace Scion::Windowing::Inputs
