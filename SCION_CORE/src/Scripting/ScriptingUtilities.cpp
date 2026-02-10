#include "Core/Scripting/ScriptingUtilities.h"
#include "Logger/Logger.h"

namespace Scion::Core::Scripting
{

namespace
{
const std::string PRIVATE_KEY = "Scion2DPrivateEncryptionKey_1234321";

std::string XorCipher( const std::string& sData, const std::string& sKey )
{
	std::string sResult{};
	sResult.reserve( sData.size() );

	for (size_t i = 0; i < sData.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>( sData[ i ] );
		unsigned char k = static_cast<unsigned char>( sKey[ i % sKey.size() ] );
		sResult.push_back( static_cast<char>( c ^ k ) );
	}

	return sResult;
}

std::string SimpleEncrypt(const std::string& sStr)
{
	return XorCipher( sStr, PRIVATE_KEY );
}

std::string SimpleDecrypt( const std::string& sStr )
{
	return XorCipher( sStr, PRIVATE_KEY );
}

}



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

	std::string sLuaMakeReadOnlyTable = R"(
		function S2D_MakeReadOnlyTable(name, entries)
			-- Create a new table (the "public" table users see)
			local tbl = {}

			-- Create the metatable that holds the actual values
			local metaTbl = {}

			-- Copy the entries into the metatable
			for k, v in pairs(entries) do
				metaTbl[k] = v
			end

			-- prevent modification of the table
			metaTbl.__newindex = function(_, _)
				error("Attempt to modify read-only table: " ..tostring(name), 2)
			end

			-- Custom index lookup: return value if found, otherwise error
			metaTbl.__index = function(_, key)
				local val = rawget(metaTbl, key)
				if val ~= nil then
					return val
				end

				error("Key [" ..tostring(key) .."] was not found in read-only table: " ..tostring(name), 2)
			end

			-- Protect the metatable from tampering
			metaTbl.__metatable = "This metatable is locked."

			-- Attach the metatable to the table
			setmetatable(tbl, metaTbl)

			return tbl			
		end
	)";

	std::string sLuaSwitchStatement = R"(
		-- Switch helper function
		function S2D_switch(value, cases, default)
			-- Validate all entries for cases
			for k, v in pairs(cases) do
				if type(v) ~= "function" then
					error("Invalid case for key ['" ..tostring(k) .."'] must be a function, got " ..type(v), 2)
				end
			end

			-- Lookup the function for this case
			local case = cases[value]

			if case then
				-- If the case is valid, run the function
				return case()
			elseif default then

				if type(default) ~= "function" then
					error("Invalid default: must be a function, got " .. type(default), 2)
				end

				-- If no case matched, run the default function (if provided)
				return default()
			end			
		end
	)";

	try
	{
		auto result = lua.safe_script(sLuaClassCode);
		result = lua.safe_script(sLuaDeepClone);
		result = lua.safe_script(sLuaShallowCopy);
		result = lua.safe_script(sLuaInsertUnique);
		result = lua.safe_script(sLuaMakeReadOnlyTable);
		result = lua.safe_script(sLuaSwitchStatement);

		lua.set_function("S2D_Encrypt", &SimpleEncrypt);
		lua.set_function("S2D_Decrypt", &SimpleDecrypt);
	}
	catch (const sol::error& err)
	{
		SCION_ERROR("Failed to create lua shallow copy code. {}", err.what());
	}

	// clang-format on
}
} // namespace Scion::Core::Scripting
