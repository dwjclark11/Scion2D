#include "Physics/ContactListener.h"
#include <Logger/Logger.h>

namespace SCION_PHYSICS {

	void ContactListener::SetUserContacts(UserData* a, UserData* b)
	{
		m_pUserDataA = a;
		m_pUserDataB = b;
	}

	void ContactListener::BeginContact(b2Contact* contact)
	{
		UserData* a_data = reinterpret_cast<UserData*>(contact->GetFixtureA()->GetUserData().pointer);
		UserData* b_data = reinterpret_cast<UserData*>(contact->GetFixtureB()->GetUserData().pointer);

		try
		{
			auto a_any = std::any_cast<ObjectData>(a_data->userData);
			auto b_any = std::any_cast<ObjectData>(b_data->userData);

			a_any.AddContact(b_any);
			b_any.AddContact(a_any);

			a_data->userData.reset();
			a_data->userData = a_any;

			b_data->userData.reset();
			b_data->userData = b_any;

			SetUserContacts(a_data, b_data);
		}
		catch (const std::bad_any_cast& ex)
		{
			SCION_ERROR("Failed to cast user contacts: {}", ex.what());
			SetUserContacts(nullptr, nullptr);
		}
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
		UserData* a_data = reinterpret_cast<UserData*>(contact->GetFixtureA()->GetUserData().pointer);
		UserData* b_data = reinterpret_cast<UserData*>(contact->GetFixtureB()->GetUserData().pointer);

		try
		{
			auto a_any = std::any_cast<ObjectData>(a_data->userData);
			auto b_any = std::any_cast<ObjectData>(b_data->userData);

			if (!a_any.RemoveContact(b_any))
			{
				// TODO: LOG ERROR
			}

			if (!b_any.RemoveContact(a_any))
			{
				// TODO: LOG ERROR
			}

			a_data->userData.reset();
			a_data->userData = a_any;

			b_data->userData.reset();
			b_data->userData = b_any;

		}
		catch (const std::bad_any_cast& ex)
		{
			SCION_ERROR("Failed to cast user contacts: {}", ex.what());
		}

		SetUserContacts(nullptr, nullptr);
	}

	void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		// TODO:
	}

	void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
		// TODO:
	}

}
