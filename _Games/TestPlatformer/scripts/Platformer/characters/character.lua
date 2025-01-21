Character = {}
Character.__index = Character

function Character:Create(def)
	local this = 
	{
		m_Def = def,
		m_EntityID = -1,
		m_StartPos = vec2(0, 0),
		m_bInitialized = false,
		m_bDead = false,
		m_bFacingLeft = false, 
		m_bJumping = false,
		m_bGrounded = true,
		m_Type = def.type or "PLAYER",
		m_Controller = StateMachine(),
		m_Name = def.name,
	}

	local entityDef = nil 
	if this.m_Type == "PLAYER" then 
		entityDef = PlayerDefs[def.name]
	elseif this.m_Type == "ENEMY" then 
		entityDef = EnemyDefs[def.name]
	end 

	assert(entityDef, string.format("Failed to get the entity def for [%s]", def.name))

	this.m_EntityID = LoadEntity(entityDef)

	assert(this.m_EntityID ~= -1, "Failed to load entity")
	assert(this.m_EntityID, "Failed to load entity")

	this.m_StartPos = this.m_Def.position or entityDef.components.transform.position
	
	if entityDef.controller then 
		self:InitStateMachine (entityDef, this)
	end
	
	setmetatable(this, self)
	return this
end

function Character:InitStateMachine (def, character)
	if character.m_bInitialized then 
		return 
	end 

	local states = nil 
	if character.m_Type == "PLAYER" then 
		assert(CharacterStates, "CharacterStates does not exist")
		states = CharacterStates 
	elseif character.m_Type == "ENEMY" then 
		assert(EnemyStates, "EnemyStates does not exist")
		states = EnemyStates 
	end 

	for _, name in ipairs(def.controller) do 
		local state = states[name]
		assert(state, string.format("State [%s] does not exist.", name))
		local instance = state:Create(character)
		character.m_Controller:add_state(instance)
	end

	character.m_Controller:change_state(def.default_state, false, nil)
	character.m_bInitialized = true

end 

