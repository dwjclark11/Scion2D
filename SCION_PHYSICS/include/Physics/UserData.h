#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <sstream>
#include <vector>

namespace SCION_PHYSICS
{
struct UserData
{
	std::any userData{};
	std::uint32_t type_id{ 0 };
};

struct ObjectData
{
	std::string tag{ "" }, group{ "" };
	bool bCollider{ false }, bTrigger{ false };
	std::uint32_t entityID{};
	std::vector<ObjectData> contactEntities;

	friend bool operator==( const ObjectData& a, const ObjectData& b );
	bool AddContact( const ObjectData& objectData );
	bool RemoveContact( const ObjectData& objectData );

	[[nodiscard]] std::string to_string() const;
};
} // namespace SCION_PHYSICS