#include "Physics/ContactListener.h"
#include <Logger/Logger.h>

#include <entt/entt.hpp>

namespace Scion::Physics
{

void ContactListener::SetUserContacts( UserData* a, UserData* b )
{
	m_pUserDataA = a;
	m_pUserDataB = b;
}

void ContactListener::BeginContact( b2Contact* contact )
{
	auto* fixtureA = contact->GetFixtureA();
	auto* fixtureB = contact->GetFixtureB();

	if ( !fixtureA || !fixtureB || !fixtureA->GetUserData().pointer || !fixtureB->GetUserData().pointer )
	{
		SetUserContacts( nullptr, nullptr );
		return;
	}

	UserData* a_data = reinterpret_cast<UserData*>( fixtureA->GetUserData().pointer );
	UserData* b_data = reinterpret_cast<UserData*>( fixtureB->GetUserData().pointer );

	constexpr auto expectedType = entt::type_hash<ObjectData>::value();

	if ( !a_data || !b_data || a_data->typeId != expectedType || b_data->typeId != expectedType )
	{
		SetUserContacts( nullptr, nullptr );
		return;
	}

	try
	{
		auto* a_any = std::any_cast<ObjectData>( &a_data->userData );
		auto* b_any = std::any_cast<ObjectData>( &b_data->userData );

		a_any->AddContact( b_any );
		b_any->AddContact( a_any );

		SetUserContacts( a_data, b_data );
	}
	catch ( const std::bad_any_cast& ex )
	{
		SCION_ERROR( "Failed to cast user contacts: {}", ex.what() );
		SetUserContacts( nullptr, nullptr );
		// Eat this exception for now
	}
}

void ContactListener::EndContact( b2Contact* contact )
{
	auto* fixtureA = contact->GetFixtureA();
	auto* fixtureB = contact->GetFixtureB();

	if ( !fixtureA || !fixtureB || !fixtureA->GetUserData().pointer || !fixtureB->GetUserData().pointer )
	{
		SetUserContacts( nullptr, nullptr );
		return;
	}

	UserData* a_data = reinterpret_cast<UserData*>( fixtureA->GetUserData().pointer );
	UserData* b_data = reinterpret_cast<UserData*>( fixtureB->GetUserData().pointer );

	constexpr auto expectedType = entt::type_hash<ObjectData>::value();

	if ( !a_data || !b_data || a_data->typeId != expectedType || b_data->typeId != expectedType )
	{
		SetUserContacts( nullptr, nullptr );
		return;
	}

	try
	{
		auto* a_any = std::any_cast<ObjectData>( &a_data->userData );
		auto* b_any = std::any_cast<ObjectData>( &b_data->userData );
		if (!a_any && b_any)
		{
			b_any->ClearContacts();
			SetUserContacts( nullptr, nullptr );
			return;
		}

		if (a_any && !b_any)
		{
			a_any->ClearContacts();
			SetUserContacts( nullptr, nullptr );
			return;
		}

		a_any->RemoveContact( b_any );
		b_any->RemoveContact( a_any );		
	}
	catch ( const std::bad_any_cast& ex )
	{
		SCION_ERROR( "Failed to cast user contacts: {}", ex.what() );
		//  Eat this exception for now
	}

	SetUserContacts( nullptr, nullptr );
}

void ContactListener::PostSolve( b2Contact* contact, const b2ContactImpulse* impulse )
{
	auto* fixtureA = contact->GetFixtureA();
	auto* fixtureB = contact->GetFixtureB();

	if ( !fixtureA || !fixtureB || !fixtureA->GetUserData().pointer || !fixtureB->GetUserData().pointer )
	{
		return;
	}

	UserData* a_data = reinterpret_cast<UserData*>( fixtureA->GetUserData().pointer );
	UserData* b_data = reinterpret_cast<UserData*>( fixtureB->GetUserData().pointer );

	constexpr auto expectedType = entt::type_hash<ObjectData>::value();

	if ( !a_data || !b_data || a_data->typeId != expectedType || b_data->typeId != expectedType )
	{
		return;
	}

	if ( !impulse )
	{
		return;
	}

	// Create impulseTable
	ContactImpulseInfo contactInfo{};

	for ( int i = 0; i < impulse->count; ++i )
	{
		contactInfo.normalImpulses.push_back( impulse->normalImpulses[ i ] );
		contactInfo.tangentImpulses.push_back( impulse->tangentImpulses[ i ] );
	}

	contactInfo.count = impulse->count;

	try
	{
		auto* a_any = std::any_cast<ObjectData>( &a_data->userData );
		auto* b_any = std::any_cast<ObjectData>( &b_data->userData );

		if ( a_any->onPostSolve.valid() )
		{
			// Dereference to pass by reference to Lua!
			auto result = a_any->onPostSolve( *b_any, contactInfo );
			if ( !result.valid() )
			{
				sol::error err = result;
				SCION_ERROR( "Lua error in PostSolve: {}", err.what() );
			}
		}

		if ( b_any->onPostSolve.valid() )
		{
			// Dereference to pass by reference to Lua!
			auto result = b_any->onPostSolve( *a_any, contactInfo );
			if ( !result.valid() )
			{
				sol::error err = result;
				SCION_ERROR( "Lua error in PostSolve: {}", err.what() );
			}
		}
	}
	catch ( const std::bad_any_cast& ex )
	{
		SCION_ERROR( "Failed to cast user contacts in PostSolve: {}", ex.what() );
	}
}

namespace
{
// onPreSolve may return:
//   - nothing / nil            -> contact proceeds unmodified
//   - true / false             -> allow / disable the contact (original contract, unchanged)
//   - a table                  -> { allowed = bool (default true), friction = number, restitution = number }
//     any field left out keeps that property at whatever it already was for this contact.
// Returns false if the contact was disabled (caller should stop -- no point applying the
// other side's overrides to a contact that's no longer going to be solved).
bool ApplyPreSolveResult( b2Contact* contact, const sol::protected_function_result& result )
{
	if ( !result.valid() )
		return true;

	sol::object retVal = result.get<sol::object>();

	if ( retVal.get_type() == sol::type::boolean )
	{
		if ( !retVal.as<bool>() )
		{
			contact->SetEnabled( false );
			return false;
		}
		return true;
	}

	if ( retVal.get_type() == sol::type::table )
	{
		sol::table t = retVal.as<sol::table>();

		if ( !t.get_or( "allowed", true ) )
		{
			contact->SetEnabled( false );
			return false;
		}

		sol::optional<float> optFriction = t[ "friction" ];
		if ( optFriction )
			contact->SetFriction( *optFriction );

		sol::optional<float> optRestitution = t[ "restitution" ];
		if ( optRestitution )
			contact->SetRestitution( *optRestitution );
	}

	return true;
}
} // namespace

void ContactListener::PreSolve( b2Contact* contact, const b2Manifold* oldManifold )
{
	auto* fixtureA = contact->GetFixtureA();
	auto* fixtureB = contact->GetFixtureB();

	if ( !fixtureA || !fixtureB || !fixtureA->GetUserData().pointer || !fixtureB->GetUserData().pointer )
	{
		return;
	}

	UserData* a_data = reinterpret_cast<UserData*>( fixtureA->GetUserData().pointer );
	UserData* b_data = reinterpret_cast<UserData*>( fixtureB->GetUserData().pointer );

	constexpr auto expectedType = entt::type_hash<ObjectData>::value();

	if ( !a_data || !b_data || a_data->typeId != expectedType || b_data->typeId != expectedType )
	{
		return;
	}

	try
	{
		auto* a_any = std::any_cast<ObjectData>( &a_data->userData );
		auto* b_any = std::any_cast<ObjectData>( &b_data->userData );

		if ( a_any->onPreSolve.valid() )
		{
			if ( !ApplyPreSolveResult( contact, a_any->onPreSolve( contact, b_any ) ) )
				return;
		}

		if ( b_any->onPreSolve.valid() )
		{
			if ( !ApplyPreSolveResult( contact, b_any->onPreSolve( contact, a_any ) ) )
				return;
		}
	}
	catch ( const std::bad_any_cast& ex )
	{
		SCION_ERROR( "Failed to cast user contacts in PreSolve: {}", ex.what() );
	}
}

} // namespace Scion::Physics
