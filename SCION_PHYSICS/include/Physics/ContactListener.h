#pragma once
#include "Box2DWrappers.h"
#include "UserData.h"

namespace SCION_PHYSICS
{
class ContactListener : public b2ContactListener
{
  private:
	UserData* m_pUserDataA{nullptr};
	UserData* m_pUserDataB{nullptr};

	void SetUserContacts(UserData* a, UserData* b);

  public:
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

	UserData* GetUserDataA() { return m_pUserDataA; }
	UserData* GetUserDataB() { return m_pUserDataB; }
};
} // namespace SCION_PHYSICS