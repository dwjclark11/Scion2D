#include "Physics/PhysicsUtilities.h"
#include "ScionUtilities/ScionUtilities.h"
#include <fmt/format.h>
#include <map>
#include <algorithm>

namespace SCION_PHYSICS
{

static const std::map<std::string, RigidBodyType> StringToRigidBodyType{ { "STATIC", RigidBodyType::STATIC },
																		 { "KINEMATIC", RigidBodyType::KINEMATIC },
																		 { "DYNAMIC", RigidBodyType::DYNAMIC } };

static const std::map<RigidBodyType, std::string> RigidBodyTypeToString{ { RigidBodyType::STATIC, "STATIC" },
																		 { RigidBodyType::KINEMATIC, "KINEMATIC" },
																		 { RigidBodyType::DYNAMIC, "DYNAMIC" } };

static std::map<std::string, FilterCategory> StringToFilterCategory{ { "NO_CATEGORY", FilterCategory::NO_CATEGORY },
																	 { "PLAYER", FilterCategory::PLAYER },
																	 { "ENEMY", FilterCategory::ENEMY },
																	 { "ITEM", FilterCategory::ITEM },
																	 { "WALLS", FilterCategory::WALLS },
																	 { "GROUND", FilterCategory::GROUND },
																	 { "TRIGGER", FilterCategory::TRIGGER },
																	 { "PROJECTILE", FilterCategory::PROJECTILE },
																	 { "CATEGORY_7", FilterCategory::CATEGORY_7 },
																	 { "CATEGORY_8", FilterCategory::CATEGORY_8 },
																	 { "CATEGORY_9", FilterCategory::CATEGORY_9 },
																	 { "CATEGORY_10", FilterCategory::CATEGORY_10 },
																	 { "CATEGORY_11", FilterCategory::CATEGORY_11 },
																	 { "CATEGORY_12", FilterCategory::CATEGORY_12 },
																	 { "CATEGORY_13", FilterCategory::CATEGORY_13 },
																	 { "CATEGORY_14", FilterCategory::CATEGORY_14 },
																	 { "CATEGORY_15", FilterCategory::CATEGORY_15 } };

static std::map<FilterCategory, std::string> FilterCategoryToString{ { FilterCategory::NO_CATEGORY, "NO_CATEGORY" },
																	 { FilterCategory::PLAYER, "PLAYER" },
																	 { FilterCategory::ENEMY, "ENEMY" },
																	 { FilterCategory::ITEM, "ITEM" },
																	 { FilterCategory::WALLS, "WALLS" },
																	 { FilterCategory::GROUND, "GROUND" },
																	 { FilterCategory::TRIGGER, "TRIGGER" },
																	 { FilterCategory::PROJECTILE, "PROJECTILE" },
																	 { FilterCategory::CATEGORY_7, "CATEGORY_7" },
																	 { FilterCategory::CATEGORY_8, "CATEGORY_8" },
																	 { FilterCategory::CATEGORY_9, "CATEGORY_9" },
																	 { FilterCategory::CATEGORY_10, "CATEGORY_10" },
																	 { FilterCategory::CATEGORY_11, "CATEGORY_11" },
																	 { FilterCategory::CATEGORY_12, "CATEGORY_12" },
																	 { FilterCategory::CATEGORY_13, "CATEGORY_13" },
																	 { FilterCategory::CATEGORY_14, "CATEGORY_14" },
																	 { FilterCategory::CATEGORY_15, "CATEGORY_15" } };

std::string GetFilterCategoryString( FilterCategory eCat )
{
	auto filterCatItr = FilterCategoryToString.find( eCat );
	if ( filterCatItr == FilterCategoryToString.end() )
		return {};

	return filterCatItr->second;
}

FilterCategory GetFilterCategoryByString( const std::string& sCat )
{
	auto filterCatItr = StringToFilterCategory.find( sCat );
	if ( filterCatItr == StringToFilterCategory.end() )
		return FilterCategory::NO_CATEGORY;

	return filterCatItr->second;
}

bool AddCustomFilterCategoryType( const std::string& sNewFilterCatType, std::string& sErrorStr )
{
	if ( StringToFilterCategory.contains( sNewFilterCatType ) )
	{
		sErrorStr = fmt::format( "Filter Category [{}] already exists.", sNewFilterCatType );
		return false;
	}

	// Check if there are any free User Categories left
	auto catItr = std::ranges::find_if( StringToFilterCategory,
										[]( const auto& pair ) { return pair.first.starts_with( "CATEGORY_" ); } );

	if ( catItr == StringToFilterCategory.end() )
	{
		sErrorStr = "There are no new filter categories available. Please remove one to add a new.";
		return false;
	}

	std::string sFilterCatToChange = catItr->first;
	FilterCategory eFilterCat = catItr->second;

	// Now we want to get the key and change the node name.
	if ( !SCION_UTIL::KeyChange( StringToFilterCategory, sFilterCatToChange, sNewFilterCatType ) )
	{
		sErrorStr = "Failed to add new filter category. Unable to change the key.";
		return false;
	}

	// Change the value of the other map as well
	FilterCategoryToString[ eFilterCat ] = sNewFilterCatType;

	return true;
}

/*
 * We don't want to expose the CATEGORY_ filters. Those are for users to define. The category value does not change;
 * however, in the Editor, the user will see the value they defined, rather than CATEGORY_[num]
 */
const std::map<FilterCategory, std::string> GetFilterCategoryToStringMap()
{
	auto filteredValues = FilterCategoryToString |
						  std::views::filter( []( auto&& pair ) { return !pair.second.starts_with( "CATEGORY_" ); } );

	std::map<FilterCategory, std::string> filteredMap{ filteredValues.begin(), filteredValues.end() };
	return filteredMap;
}

std::string GetRigidBodyTypeString( RigidBodyType eRigidType )
{
	auto rigidItr = RigidBodyTypeToString.find( eRigidType );
	if ( rigidItr == RigidBodyTypeToString.end() )
		return {};

	return rigidItr->second;
}

RigidBodyType GetRigidBodyTypeByString( const std::string sRigidType )
{
	auto rigidItr = StringToRigidBodyType.find( sRigidType );
	if ( rigidItr == StringToRigidBodyType.end() )
		return RigidBodyType::STATIC;

	return rigidItr->second;
}
const std::map<RigidBodyType, std::string>& GetRigidBodyStringMap()
{
	return RigidBodyTypeToString;
}

} // namespace SCION_PHYSICS
