#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <sstream>
#include <vector>
#include <entt/entt.hpp>

namespace SCION_PHYSICS
{
struct UserData
{
	std::any userData{};
	std::uint32_t type_id{ 0 };
};

/*
* ObjectData
* Currently this struct is used for all Rigidbodies in Scion2D.
* This is waht the user data is set to and checked for in the contact listerer bindings.
* You may need a specific user data setup for your own specific needs; however,
* you can always use the tag and group to do different functions on the body as needed.
*
* For instance, in the lua scripts:
*
*
	function RunSpikeDeath(objectA, objectB)
		-- TODO: Kill the player
		-- TODO: Add blood to the spikes
	end

	function UpdateContacts()
		local uda, udb = ContactListener.get_user_data()
		if uda and udb then
			if uda.tag == "player" and udb.group == "spikes" then
				RunSpikeDeath(uda, udb)
			end
		end
	end
*/

struct ObjectData
{
	std::string tag{ "" };
	std::string group{ "" };
	bool bCollider{ false };
	bool bTrigger{ false };
	bool bIsFriendly{ false };
	std::uint32_t entityID{};

	ObjectData() = default;
	ObjectData( const std::string& tag, const std::string& group, bool collider, bool trigger, bool friendly,
				std::uint32_t entityId = entt::null );

	inline const std::vector<const ObjectData*>& GetContactEntities() const { return contactEntities; }

	friend bool operator==( const ObjectData& a, const ObjectData& b );
	[[nodiscard]] std::string to_string() const;

  private:
	bool AddContact( const ObjectData* objectData );
	bool RemoveContact( const ObjectData* objectData );
	void ClearContacts() { contactEntities.clear(); }

	friend class ContactListener;

  private:
	std::vector<const ObjectData*> contactEntities;
};
} // namespace SCION_PHYSICS
