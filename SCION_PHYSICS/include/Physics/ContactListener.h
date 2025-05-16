#pragma once
#include "Box2DWrappers.h"
#include "UserData.h"

namespace SCION_PHYSICS
{
class ContactListener : public b2ContactListener
{
  public:
	/*
	 * @brief Called when two fixtures begin to touch.
	 * @param b2Contact*
	 */
	void BeginContact( b2Contact* contact ) override;

	/*
	 * @brief Called when two fixtures cease to touch.
	 * @param b2Contact* contact
	 */
	void EndContact( b2Contact* contact ) override;

	/*
	 * @brief Allows you inspect a contact after the solver is finished. This is useful for inspecting impulses.
	 * @brief Note: the contact manifold does not include time of impact impulses, which can be arbitrarily large if the
	 * sub-step is small. Hence the impulse is provided explicitly in a separate data structure.
	 * @brief Note: this is only called for contacts that are touching, solid, and awake.
	 */
	void PostSolve( b2Contact* contact, const b2ContactImpulse* impulse ) override;

	/*
	 * @brief This is called after a contact is updated. This allows you to inspect a contact before it goes to the
	 * solver. If you are careful, you can modify the contact manifold (e.g. disable contact). A copy of the old
	 * manifold is provided so that you can detect changes.
	 * @brief Note: this is called only for awake bodies.
	 * @brief Note: this is called even when the number of contact points is zero. Note: this is not called for sensors.
	 * @brief Note: if you set the number of contact points to zero, you will not get an EndContact callback.
	 * However, you may get a BeginContact callback the next step.
	 */
	void PreSolve( b2Contact* contact, const b2Manifold* oldManifold ) override;

	UserData* GetUserDataA() { return m_pUserDataA; }
	UserData* GetUserDataB() { return m_pUserDataB; }

  private:
	void SetUserContacts( UserData* a, UserData* b );

  private:
	UserData* m_pUserDataA{ nullptr };
	UserData* m_pUserDataB{ nullptr };
};
} // namespace SCION_PHYSICS
