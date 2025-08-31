#include "Core/Scripting/ScriptingUtilities.h"
#include "Logger/Logger.h"

namespace SCION_CORE::Scripting
{
void ScriptingHelpers::CreateLuaHelpers( sol::state& lua )
{
	// clang-format off
	
	std::string sLuaClassCode = R"(
		function S2D_Class(className, base)
			-- Create a new table that will represent our class
			local cls = {}
			-- Set the __index metamethod so method lookups refer to the class
			cls.__index = cls 	
	
			-- If a base class is provided, set up inheritance
			if base then
				-- Delegate method lookup to base if not found in cls
				setmetatable(cls, { __index = base })
				-- Provide a reference to the base class (useful for base methods)
				cls.super = base 					 	
			end 
	
			-- Define a contructor function
			function cls:Create(...)
				-- Create a new object with cls as its metatable
				local obj = setmetatable({}, cls)
				-- If an init function exists on the object (like a contstuctor)
				if obj.Init then
					-- Call init with any arguments passed into create
					obj:Init(...)					
				end
				-- Returns the new object
				return obj							
			end 

			-- Returns the new class
			return cls 								
		end
	)";

	std::string sLuaDeepClone = R"(
		function S2D_DeepClone(t)
			-- Create a new empty table to store the clone
			local clone = {}
			-- Iterate over all key-value pairs in the input table
			for k, v in pairs(t) do
				-- If the value is also a table, perform a recursive deep clone
				if type(v) == "table" then
					-- Recursively clone the table value
					clone[k] = S2D_DeepClone(v)
				else
					-- For non-table values (number, string, boolean, function, etc.), copy directly
					clone[k] = v
				end
			end
			-- Set the metatable of the cloned table to match that of the original table
			-- This is important for preserving behavior like methods in class-like tables
			setmetatable(clone, getmetatable(t))
			-- Return the fully cloned table
			return clone
		end
	)";

	std::string sLuaShallowCopy = R"(
		function S2D_ShallowClone(t)
			local clone = {}
			for k, v in pairs(t) do
				clone[k] = v
			end

			setmetatable(clone, getmetatable(t))
			return clone
		end
	)";

	std::string sLuaInsertUnique = R"(
		function S2D_InsertUnique(tbl, value)
			for _, v in pairs(tbl) do 
				if v == value then 
					return -- Already in table
				end 
			end 
	
			table.insert(tbl, value)
		end
	)";

	try
	{
		auto result = lua.safe_script(sLuaClassCode);
		result = lua.safe_script(sLuaDeepClone);
		result = lua.safe_script(sLuaShallowCopy);
		result = lua.safe_script(sLuaInsertUnique);
	}
	catch (const sol::error& err)
	{
		SCION_ERROR("Failed to create lua shallow copy code. {}", err.what());
	}

	// clang-format on
}
} // namespace SCION_CORE::Scripting
