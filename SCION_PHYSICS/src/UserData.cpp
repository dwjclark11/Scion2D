#include "Physics/UserData.h"
#include <algorithm> // find_if

namespace SCION_PHYSICS
{
bool ObjectData::AddContact( const ObjectData& objectData )
{
	if ( tag.empty() && group.empty() )
		return false;

	if ( objectData.tag.empty() && objectData.group.empty() )
		return false;

	if ( objectData.tag == tag && objectData.group == group )
		return false;

	if ( bIsFriendly && objectData.bIsFriendly && bTrigger && objectData.bTrigger )
		return false;

	auto contactItr = std::find_if( contactEntities.begin(), contactEntities.end(), [ & ]( ObjectData& contactInfo ) {
		return contactInfo == objectData;
	} );

	if ( contactItr != contactEntities.end() )
		return false;

	contactEntities.push_back( objectData );
	return true;
}

bool ObjectData::RemoveContact( const ObjectData& objectData )
{
	if ( objectData.tag.empty() && objectData.group.empty() )
		return true;

	auto contactItr = std::remove_if( contactEntities.begin(), contactEntities.end(), [ & ]( ObjectData& contactInfo ) {
		return contactInfo == objectData;
	} );

	if ( contactItr == contactEntities.end() )
		return false;

	contactEntities.erase( contactItr );
	return true;
}

std::string ObjectData::to_string() const
{
	std::stringstream ss;
	ss << "==== Object Data ==== \n"
	   << std::boolalpha << "Tag: " << tag << "\n"
	   << "Group: " << group << "\n"
	   << "bCollider: " << bCollider << "\n"
	   << "bTrigger: " << bTrigger << "\n"
	   << "EntityID: " << entityID << "\n";

	return ss.str();
}

bool operator==( const ObjectData& a, const ObjectData& b )
{
	return a.bCollider == b.bCollider && a.bTrigger == b.bTrigger && a.tag == b.tag && a.group == b.group &&
		   a.entityID == b.entityID;
}

} // namespace SCION_PHYSICS
