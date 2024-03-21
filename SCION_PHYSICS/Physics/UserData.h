#pragma once
#include <any>
#include <string>
#include <sstream>

namespace SCION_PHYSICS {
	struct UserData
	{
		std::any userData{};
		std::uint32_t type_id{0};
	};

	struct ObjectData
	{
		std::string tag{""}, group{ "" };
		bool bCollider{ false }, bTrigger{ false };
		std::uint32_t entityID{};

		[[nodiscard]] std::string to_string() const
		{
			std::stringstream ss;
			ss <<
				"==== Object Data ==== \n" << std::boolalpha <<
				"Tag: " << tag << "\n" <<
				"Group: " << group << "\n" <<
				"bCollider: " << bCollider << "\n" <<
				"bTrigger: " << bTrigger << "\n" <<
				"EntityID: " << entityID << "\n";

			return ss.str();
		}
	};
}