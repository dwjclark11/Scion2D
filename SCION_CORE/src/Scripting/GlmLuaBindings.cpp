#include "Core/Scripting/GlmLuaBindings.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

namespace SCION_CORE::Scripting
{

// glm::vec2
void CreateVec2Bind( sol::state& lua )
{
	// multiplier overloads
	auto vec2_multiply_overloads = sol::overload( []( const glm::vec2& v1, const glm::vec2& v2 ) { return v1 * v2; },
												  []( const glm::vec2& v1, float value ) { return v1 * value; },
												  []( float value, const glm::vec2& v1 ) { return v1 * value; } );

	// divider overloads
	auto vec2_divide_overloads = sol::overload( []( const glm::vec2& v1, const glm::vec2& v2 ) { return v1 / v2; },
												[]( const glm::vec2& v1, float value ) { return v1 / value; },
												[]( float value, const glm::vec2& v1 ) { return v1 / value; } );

	// addition overloads
	auto vec2_addition_overloads = sol::overload( []( const glm::vec2& v1, const glm::vec2& v2 ) { return v1 + v2; },
												  []( const glm::vec2& v1, float value ) { return v1 + value; },
												  []( float value, const glm::vec2& v1 ) { return v1 + value; } );

	// subtraction overloads
	auto vec2_subtraction_overloads = sol::overload( []( const glm::vec2& v1, const glm::vec2& v2 ) { return v1 - v2; },
													 []( const glm::vec2& v1, float value ) { return v1 - value; },
													 []( float value, const glm::vec2& v1 ) { return v1 - value; } );

	// create vec2 usertype
	lua.new_usertype<glm::vec2>(
		"vec2",
		sol::call_constructor,
		sol::constructors<glm::vec2( float ), glm::vec2( float, float )>(),
		"x",
		&glm::vec2::x,
		"y",
		&glm::vec2::y,
		sol::meta_function::multiplication,
		vec2_multiply_overloads,
		sol::meta_function::division,
		vec2_divide_overloads,
		sol::meta_function::addition,
		vec2_addition_overloads,
		sol::meta_function::subtraction,
		vec2_subtraction_overloads,
		"length",
		[]( const glm::vec2& v ) { return glm::length( v ); },
		"lengthSq",
		[]( const glm::vec2& v ) { return glm::length2( v ); },
		"normalize",
		[]( const glm::vec2& v1 ) { return glm::normalize( v1 ); },
		"normalize2",
		[]( const glm::vec2& v1, const glm::vec2& v2 ) { return glm::normalize( v2 - v1 ); },
		"nearly_zero_x",
		[]( const glm::vec2& v ) { return glm::epsilonEqual( v.x, 0.f, 0.001f ); },
		"nearly_zero_y",
		[]( const glm::vec2& v ) { return glm::epsilonEqual( v.y, 0.f, 0.001f ); } );
}

// glm::vec3
void CreateVec3Bind( sol::state& lua )
{
	// multiplier overloads
	auto vec3_multiply_overloads = sol::overload( []( const glm::vec3& v1, const glm::vec3& v2 ) { return v1 * v2; },
												  []( const glm::vec3& v1, float value ) { return v1 * value; },
												  []( float value, const glm::vec3& v1 ) { return v1 * value; } );

	// divider overloads
	auto vec3_divide_overloads = sol::overload( []( const glm::vec3& v1, const glm::vec3& v2 ) { return v1 / v2; },
												[]( const glm::vec3& v1, float value ) { return v1 / value; },
												[]( float value, const glm::vec3& v1 ) { return v1 / value; } );

	// addition overloads
	auto vec3_addition_overloads = sol::overload( []( const glm::vec3& v1, const glm::vec3& v2 ) { return v1 + v2; },
												  []( const glm::vec3& v1, float value ) { return v1 + value; },
												  []( float value, const glm::vec3& v1 ) { return v1 + value; } );

	// subtraction overloads
	auto vec3_subtraction_overloads = sol::overload( []( const glm::vec3& v1, const glm::vec3& v2 ) { return v1 - v2; },
													 []( const glm::vec3& v1, float value ) { return v1 - value; },
													 []( float value, const glm::vec3& v1 ) { return v1 - value; } );

	// create vec3 usertype
	lua.new_usertype<glm::vec3>(
		"vec3",
		sol::call_constructor,
		sol::constructors<glm::vec3( float ), glm::vec3( float, float, float )>(),
		"x",
		&glm::vec3::x,
		"y",
		&glm::vec3::y,
		"z",
		&glm::vec3::z,
		sol::meta_function::multiplication,
		vec3_multiply_overloads,
		sol::meta_function::division,
		vec3_divide_overloads,
		sol::meta_function::addition,
		vec3_addition_overloads,
		sol::meta_function::subtraction,
		vec3_subtraction_overloads,
		"length",
		[]( const glm::vec3& v ) { return glm::length( v ); },
		"lengthSq",
		[]( const glm::vec3& v ) { return glm::length2( v ); },
		"normalize",
		[]( const glm::vec3& v1 ) { return glm::normalize( v1 ); },
		"normalize2",
		[]( const glm::vec3& v1, const glm::vec3& v2 ) { return glm::normalize( v2 - v1 ); },
		"nearly_zero_x",
		[]( const glm::vec3& v ) { return glm::epsilonEqual( v.x, 0.f, 0.001f ); },
		"nearly_zero_y",
		[]( const glm::vec3& v ) { return glm::epsilonEqual( v.y, 0.f, 0.001f ); },
		"nearly_zero_z",
		[]( const glm::vec3& v ) { return glm::epsilonEqual( v.z, 0.f, 0.001f ); } );
}

// glm::vec4
void CreateVec4Bind( sol::state& lua )
{
	// multiplier overloads
	auto vec4_multiply_overloads = sol::overload( []( const glm::vec4& v1, const glm::vec4& v2 ) { return v1 * v2; },
												  []( const glm::vec4& v1, float value ) { return v1 * value; },
												  []( float value, const glm::vec4& v1 ) { return v1 * value; } );

	// divider overloads
	auto vec4_divide_overloads = sol::overload( []( const glm::vec4& v1, const glm::vec4& v2 ) { return v1 / v2; },
												[]( const glm::vec4& v1, float value ) { return v1 / value; },
												[]( float value, const glm::vec4& v1 ) { return v1 / value; } );

	// addition overloads
	auto vec4_addition_overloads = sol::overload( []( const glm::vec4& v1, const glm::vec4& v2 ) { return v1 + v2; },
												  []( const glm::vec4& v1, float value ) { return v1 + value; },
												  []( float value, const glm::vec4& v1 ) { return v1 + value; } );

	// subtraction overloads
	auto vec4_subtraction_overloads = sol::overload( []( const glm::vec4& v1, const glm::vec4& v2 ) { return v1 - v2; },
													 []( const glm::vec4& v1, float value ) { return v1 - value; },
													 []( float value, const glm::vec4& v1 ) { return v1 - value; } );

	// create vec4 usertype
	lua.new_usertype<glm::vec4>(
		"vec4",
		sol::call_constructor,
		sol::constructors<glm::vec4( float ), glm::vec4( float, float, float, float )>(),
		"x",
		&glm::vec4::x,
		"y",
		&glm::vec4::y,
		"z",
		&glm::vec4::z,
		"w",
		&glm::vec4::w,
		sol::meta_function::multiplication,
		vec4_multiply_overloads,
		sol::meta_function::division,
		vec4_divide_overloads,
		sol::meta_function::addition,
		vec4_addition_overloads,
		sol::meta_function::subtraction,
		vec4_subtraction_overloads,
		"length",
		[]( const glm::vec4& v ) { return glm::length( v ); },
		"lengthSq",
		[]( const glm::vec4& v ) { return glm::length2( v ); },
		"normalize",
		[]( const glm::vec4& v1 ) { return glm::normalize( v1 ); },
		"normalize2",
		[]( const glm::vec4& v1, const glm::vec4& v2 ) { return glm::normalize( v2 - v1 ); },
		"nearly_zero_x",
		[]( const glm::vec4& v ) { return glm::epsilonEqual( v.x, 0.f, 0.001f ); },
		"nearly_zero_y",
		[]( const glm::vec4& v ) { return glm::epsilonEqual( v.y, 0.f, 0.001f ); },
		"nearly_zero_z",
		[]( const glm::vec4& v ) { return glm::epsilonEqual( v.z, 0.f, 0.001f ); },
		"nearly_zero_w",
		[]( const glm::vec4& v ) { return glm::epsilonEqual( v.w, 0.f, 0.001f ); } );
}

/*
 * Some helper math functions
 */
void MathFreeFunctions( sol::state& lua )
{
	lua.set_function( "distance",
					  sol::overload( []( glm::vec2& a, glm::vec2& b ) { return glm::distance( a, b ); },
									 []( glm::vec3& a, glm::vec3& b ) { return glm::distance( a, b ); },
									 []( glm::vec4& a, glm::vec4& b ) { return glm::distance( a, b ); } ) );

	lua.set_function( "lerp", []( float a, float b, float t ) { return std::lerp( a, b, t ); } );
	lua.set_function(
		"clamp",
		sol::overload( []( float value, float min, float max ) { return std::clamp( value, min, max ); },
					   []( double value, double min, double max ) { return std::clamp( value, min, max ); },
					   []( int value, int min, int max ) { return std::clamp( value, min, max ); } ) );

	lua.set_function( "distance",
					  sol::overload( []( glm::vec2& a, glm::vec2& b ) { return glm::distance( a, b ); },
									 []( glm::vec3& a, glm::vec3& b ) { return glm::distance( a, b ); },
									 []( glm::vec4& a, glm::vec4& b ) { return glm::distance( a, b ); } ) );

	lua.set_function( "nearly_zero",
					  sol::overload(
						  []( const glm::vec2& v ) {
							  return glm::epsilonEqual( v.x, 0.f, 0.001f ) && glm::epsilonEqual( v.y, 0.f, 0.001f );
						  },
						  []( const glm::vec3& v ) {
							  return glm::epsilonEqual( v.x, 0.f, 0.001f ) && glm::epsilonEqual( v.y, 0.f, 0.001f ) &&
									 glm::epsilonEqual( v.z, 0.f, 0.001f );
						  } ) );
}

void MathConstants( sol::state& lua )
{
	lua.set( "SCION_PI", 3.14159265359f );
	lua.set( "SCION_TWO_PI", 6.28318530717f );
	lua.set( "SCION_PI_SQUARED", 9.86960440108f );
	lua.set( "SCION_PI_OVER_2", 1.57079632679f );
	lua.set( "SCION_PI_OVER_4", 0.78539816339f );
	lua.set( "SCION_PHI", 1.6180339887498948482045868343656381f );
	lua.set( "SCION_EULERS", 2.71828182845904523536f );

	lua.set( "SCION_SQRT_2", 1.4142135623730950488016887242097f );
	lua.set( "SCION_SQRT_3", 1.7320508075688772935274463415059f );
	lua.set( "SCION_INV_SQRT_2", 0.70710678118654752440084436210485f );
	lua.set( "SCION_INV_SQRT_3", 0.57735026918962576450914878050196f );
}

void GLMBindings::CreateGLMBindings( sol::state& lua )
{
	CreateVec2Bind( lua );
	CreateVec3Bind( lua );
	CreateVec4Bind( lua );

	MathFreeFunctions( lua );
	MathConstants( lua );
}
} // namespace SCION_CORE::Scripting
