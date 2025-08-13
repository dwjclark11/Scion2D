#include "Core/ECS/Components/ComponentSerializer.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "ScionFilesystem/Serializers/JSONSerializer.h"
#include "ScionFilesystem/Serializers/LuaSerializer.h"
#include "Physics/PhysicsUtilities.h"

using namespace SCION_PHYSICS;

namespace SCION_CORE::ECS
{
void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer,
											  const TransformComponent& transform )
{
	serializer.StartNewObject( "transform" )
		.StartNewObject( "position" )
		.AddKeyValuePair( "x", transform.position.x )
		.AddKeyValuePair( "y", transform.position.y )
		.EndObject()
		.StartNewObject( "scale" )
		.AddKeyValuePair( "x", transform.scale.x )
		.AddKeyValuePair( "y", transform.scale.y )
		.EndObject()
		.AddKeyValuePair( "rotation", transform.rotation )
		.EndObject();
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer,
											  const SpriteComponent& sprite )
{
	serializer.StartNewObject( "sprite" )
		.AddKeyValuePair( "width", sprite.width )
		.AddKeyValuePair( "height", sprite.height )
		.AddKeyValuePair( "startX", sprite.start_x )
		.AddKeyValuePair( "startY", sprite.start_y )
		.AddKeyValuePair( "layer", sprite.layer )
		.AddKeyValuePair( "sTexture", sprite.sTextureName )
		.StartNewObject( "uvs" )
		.AddKeyValuePair( "u", sprite.uvs.u )
		.AddKeyValuePair( "v", sprite.uvs.v )
		.AddKeyValuePair( "uv_width", sprite.uvs.uv_width )
		.AddKeyValuePair( "uv_height", sprite.uvs.uv_height )
		.EndObject()
		.StartNewObject( "color" )
		.AddKeyValuePair( "r", sprite.color.r )
		.AddKeyValuePair( "g", sprite.color.g )
		.AddKeyValuePair( "b", sprite.color.b )
		.AddKeyValuePair( "a", sprite.color.a )
		.EndObject()
		.AddKeyValuePair( "bHidden", sprite.bHidden )
		.AddKeyValuePair( "bIsoMetric", sprite.bIsoMetric )
		.AddKeyValuePair( "isoCellX", sprite.isoCellX )
		.AddKeyValuePair( "isoCellY", sprite.isoCellY )
		.EndObject();
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer,
											  const AnimationComponent& animation )
{
	serializer.StartNewObject( "animation" )
		.AddKeyValuePair( "numFrames", animation.numFrames )
		.AddKeyValuePair( "frameRate", animation.frameRate )
		.AddKeyValuePair( "frameOffset", animation.frameOffset )
		.AddKeyValuePair( "bVertical", animation.bVertical )
		.AddKeyValuePair( "bLooped", animation.bLooped )
		.EndObject();
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer,
											  const BoxColliderComponent& boxCollider )
{
	serializer.StartNewObject( "boxCollider" )
		.AddKeyValuePair( "width", boxCollider.width )
		.AddKeyValuePair( "height", boxCollider.height )
		.StartNewObject( "offset" )
		.AddKeyValuePair( "x", boxCollider.offset.x )
		.AddKeyValuePair( "y", boxCollider.offset.y )
		.EndObject()
		.EndObject();
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer,
											  const CircleColliderComponent& circleCollider )
{
	serializer.StartNewObject( "circleCollider" )
		.AddKeyValuePair( "radius", circleCollider.radius )
		.StartNewObject( "offset" )
		.AddKeyValuePair( "x", circleCollider.offset.x )
		.AddKeyValuePair( "y", circleCollider.offset.y )
		.EndObject()
		.EndObject();
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const TextComponent& text )
{
	serializer
		.StartNewObject( "text" ) // Start text table
		.AddKeyValuePair( "text", text.sTextStr )
		.AddKeyValuePair( "fontName", text.sFontName )
		.AddKeyValuePair( "padding", text.padding )
		.AddKeyValuePair( "wrap", text.wrap )
		.AddKeyValuePair( "bHidden", text.bHidden )
		.StartNewObject( "color" ) // Start Color table
		.AddKeyValuePair( "r", static_cast<int>( text.color.r ) )
		.AddKeyValuePair( "g", static_cast<int>( text.color.g ) )
		.AddKeyValuePair( "b", static_cast<int>( text.color.b ) )
		.AddKeyValuePair( "a", static_cast<int>( text.color.a ) )
		.EndObject();		// End Color table
	serializer.EndObject(); // End Text table
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer,
											  const PhysicsComponent& physics )
{
	const auto& attributes = physics.GetAttributes();

	serializer.StartNewObject( "physics" )
		.StartNewObject( "attributes" )
		.AddKeyValuePair( "type", GetRigidBodyTypeString( attributes.eType ) )
		.AddKeyValuePair( "density", attributes.density )
		.AddKeyValuePair( "friction", attributes.friction )
		.AddKeyValuePair( "restitution", attributes.restitution )
		.AddKeyValuePair( "restitutionThreshold", attributes.restitutionThreshold )
		.AddKeyValuePair( "radius", attributes.radius )
		.AddKeyValuePair( "gravityScale", attributes.gravityScale )
		.StartNewObject( "position" )
		.AddKeyValuePair( "x", attributes.position.x )
		.AddKeyValuePair( "y", attributes.position.y )
		.EndObject()
		.StartNewObject( "scale" )
		.AddKeyValuePair( "x", attributes.scale.x )
		.AddKeyValuePair( "y", attributes.scale.y )
		.EndObject()
		.StartNewObject( "boxSize" )
		.AddKeyValuePair( "x", attributes.boxSize.x )
		.AddKeyValuePair( "y", attributes.boxSize.y )
		.EndObject()
		.StartNewObject( "offset" )
		.AddKeyValuePair( "x", attributes.offset.x )
		.AddKeyValuePair( "y", attributes.offset.y )
		.EndObject()
		.AddKeyValuePair( "bCircle", attributes.bCircle )
		.AddKeyValuePair( "bBoxShape", attributes.bBoxShape )
		.AddKeyValuePair( "bFixedRotation", attributes.bFixedRotation )
		.AddKeyValuePair( "bIsSensor", attributes.bIsSensor )
		.AddKeyValuePair( "filterCategory", static_cast<unsigned>( attributes.filterCategory ) )
		.AddKeyValuePair( "filterMask", static_cast<unsigned>( attributes.filterMask ) )
		.AddKeyValuePair( "groupIndex", static_cast<int>( attributes.groupIndex ) )
		.StartNewObject( "objectData" )
		.AddKeyValuePair( "tag", attributes.objectData.tag )
		.AddKeyValuePair( "group", attributes.objectData.group )
		.AddKeyValuePair( "bCollider", attributes.objectData.bCollider )
		.AddKeyValuePair( "bTrigger", attributes.objectData.bTrigger )
		.AddKeyValuePair( "bIsFriendly", attributes.objectData.bIsFriendly )
		.EndObject()
		.EndObject()
		.EndObject();
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer,
											  const RigidBodyComponent& rigidBody )
{
	serializer
		.StartNewObject( "rigidBody" )	 // Start rigidbody table
		.StartNewObject( "maxVelocity" ) // Start velocity table
		.AddKeyValuePair( "x", rigidBody.maxVelocity.x )
		.AddKeyValuePair( "y", rigidBody.maxVelocity.y )
		.EndObject()  // end velocity table
		.EndObject(); // End rigid body table
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const Identification& id )
{
	serializer
		.StartNewObject( "id" ) // Start id table
		.AddKeyValuePair( "name", id.name )
		.AddKeyValuePair( "group", id.group )
		.EndObject(); // End id table
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const UIComponent& id )
{
	serializer.StartNewObject( "ui" ).EndObject();
	// TODO: Add more members as needed.
	// Currently the UIComponent only has a type; however, it is not currently used.
	// The UI Component is currently only used as a flag for UI Rendering
}

void ComponentSerializer::DeserializeComponent( const rapidjson::Value& jsonValue, TransformComponent& transform )
{
	transform.position =
		glm::vec2{ jsonValue[ "position" ][ "x" ].GetFloat(), jsonValue[ "position" ][ "y" ].GetFloat() };

	transform.scale = glm::vec2{ jsonValue[ "scale" ][ "x" ].GetFloat(), jsonValue[ "scale" ][ "y" ].GetFloat() };
	transform.rotation = jsonValue[ "rotation" ].GetFloat();
}

void ComponentSerializer::DeserializeComponent( const rapidjson::Value& jsonValue, SpriteComponent& sprite )
{
	sprite.width = jsonValue[ "width" ].GetFloat();
	sprite.height = jsonValue[ "height" ].GetFloat();

	sprite.uvs = UVs{ .u = jsonValue[ "uvs" ][ "u" ].GetFloat(),
					  .v = jsonValue[ "uvs" ][ "v" ].GetFloat(),
					  .uv_width = jsonValue[ "uvs" ][ "uv_width" ].GetFloat(),
					  .uv_height = jsonValue[ "uvs" ][ "uv_height" ].GetFloat() };

	sprite.start_x = jsonValue[ "startX" ].GetInt();
	sprite.start_y = jsonValue[ "startY" ].GetInt();
	sprite.layer = jsonValue[ "layer" ].GetInt(), sprite.bHidden = jsonValue[ "bHidden" ].GetBool();
	sprite.sTextureName = jsonValue[ "sTexture" ].GetString();

	// Check if sprite should be isometic
	if (jsonValue.HasMember("bIsoMetric"))
	{
		sprite.bIsoMetric = jsonValue[ "bIsoMetric" ].GetBool();
		if (jsonValue.HasMember("isoCellX"))
		{
			sprite.isoCellX = jsonValue[ "isoCellX" ].GetInt();
		}
		if ( jsonValue.HasMember( "isoCellY" ) )
		{
			sprite.isoCellY = jsonValue[ "isoCellY" ].GetInt();
		}
	}
}

void ComponentSerializer::DeserializeComponent( const rapidjson::Value& jsonValue, AnimationComponent& animation )
{
	animation.numFrames = jsonValue[ "numFrames" ].GetInt();
	animation.frameRate = jsonValue[ "frameRate" ].GetInt();
	animation.frameOffset = jsonValue[ "frameOffset" ].GetInt();
	animation.bVertical = jsonValue[ "bVertical" ].GetBool();
	animation.bLooped = jsonValue[ "bLooped" ].GetBool();
}

void ComponentSerializer::DeserializeComponent( const rapidjson::Value& jsonValue, BoxColliderComponent& boxCollider )
{
	boxCollider.width = jsonValue[ "width" ].GetInt();
	boxCollider.height = jsonValue[ "height" ].GetInt();
	boxCollider.offset = glm::vec2{ jsonValue[ "offset" ][ "x" ].GetFloat(), jsonValue[ "offset" ][ "y" ].GetFloat() };
}

void ComponentSerializer::DeserializeComponent( const rapidjson::Value& jsonValue,
												CircleColliderComponent& circleCollider )
{
	circleCollider.radius = jsonValue[ "radius" ].GetFloat();
	circleCollider.offset =
		glm::vec2{ jsonValue[ "offset" ][ "x" ].GetFloat(), jsonValue[ "offset" ][ "y" ].GetFloat() };
}

void ComponentSerializer::DeserializeComponent( const rapidjson::Value& jsonValue, TextComponent& text )
{
	text.sTextStr = jsonValue[ "text" ].GetString();
	text.sFontName = jsonValue[ "fontName" ].GetString();
	text.padding = jsonValue[ "padding" ].GetInt();
	text.wrap = jsonValue[ "wrap" ].GetFloat();
	text.bHidden = jsonValue[ "bHidden" ].GetBool();

	text.color = SCION_RENDERING::Color{ .r = static_cast<GLubyte>( jsonValue[ "color" ][ "r" ].GetInt() ),
										 .g = static_cast<GLubyte>( jsonValue[ "color" ][ "g" ].GetInt() ),
										 .b = static_cast<GLubyte>( jsonValue[ "color" ][ "b" ].GetInt() ),
										 .a = static_cast<GLubyte>( jsonValue[ "color" ][ "a" ].GetInt() ) };
}

void ComponentSerializer::DeserializeComponent( const rapidjson::Value& jsonValue, PhysicsComponent& physics )
{
	if ( jsonValue.HasMember( "attributes" ) )
	{
		const auto& attr = jsonValue[ "attributes" ];
		PhysicsAttributes attributes{
			.eType = GetRigidBodyTypeByString( attr[ "type" ].GetString() ),
			.density = attr[ "density" ].GetFloat(),
			.friction = attr[ "friction" ].GetFloat(),
			.restitution = attr[ "restitution" ].GetFloat(),
			.restitutionThreshold = attr[ "restitutionThreshold" ].GetFloat(),
			.radius = attr[ "radius" ].GetFloat(),
			.gravityScale = attr[ "gravityScale" ].GetFloat(),
			.position = glm::vec2{ attr[ "position" ][ "x" ].GetFloat(), attr[ "position" ][ "y" ].GetFloat() },
			.scale = glm::vec2{ attr[ "scale" ][ "x" ].GetFloat(), attr[ "scale" ][ "y" ].GetFloat() },
			.boxSize = glm::vec2{ attr[ "boxSize" ][ "x" ].GetFloat(), attr[ "boxSize" ][ "y" ].GetFloat() },
			.offset = glm::vec2{ attr[ "offset" ][ "x" ].GetFloat(), attr[ "offset" ][ "y" ].GetFloat() },
			.bCircle = attr[ "bCircle" ].GetBool(),
			.bBoxShape = attr[ "bBoxShape" ].GetBool(),
			.bFixedRotation = attr[ "bFixedRotation" ].GetBool(),
			.bIsSensor = attr[ "bIsSensor" ].GetBool(),
			.filterCategory = static_cast<uint16_t>( attr[ "filterCategory" ].GetUint() ),
			.filterMask = static_cast<uint16_t>( attr[ "filterMask" ].GetUint() ),
			.groupIndex = static_cast<int16_t>( attr[ "groupIndex" ].GetInt() ),
			.objectData = ObjectData{ attr[ "objectData" ][ "tag" ].GetString(),
									  attr[ "objectData" ][ "group" ].GetString(),
									  attr[ "objectData" ][ "bCollider" ].GetBool(),
									  attr[ "objectData" ][ "bTrigger" ].GetBool(),
									  attr[ "objectData" ][ "bIsFriendly" ].GetBool() }
		};

		physics.GetChangableAttributes() = attributes;
	}
}

void ComponentSerializer::DeserializeComponent( const rapidjson::Value& jsonValue, RigidBodyComponent& rigidBody )
{
	rigidBody.maxVelocity.x = jsonValue[ "maxVelocity" ][ "x" ].GetFloat();
	rigidBody.maxVelocity.y = jsonValue[ "maxVelocity" ][ "y" ].GetFloat();
}

void ComponentSerializer::DeserializeComponent( const rapidjson::Value& jsonValue, Identification& id )
{
	id.name = jsonValue[ "name" ].GetString();
	id.group = jsonValue[ "group" ].GetString();
}

void ComponentSerializer::DeserializeComponent( const rapidjson::Value& jsonValue, UIComponent& ui )
{
	ui.eType = UIObjectType::NO_TYPE;
	// TODO: Add more members as needed.
	// Currently the UIComponent only has a type; however, it is not currently used.
	// The UI Component is currently only used as a flag for UI Rendering
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer,
											  const TransformComponent& transform )
{
	serializer.StartNewTable( "transform" )
		.StartNewTable( "position", true )
		.AddKeyValuePair( "x", transform.position.x, false )
		.AddKeyValuePair( "y", transform.position.y, false, true )
		.EndTable( false )
		.StartNewTable( "scale" )
		.AddKeyValuePair( "x", transform.scale.x, false )
		.AddKeyValuePair( "y", transform.scale.y, false, true )
		.EndTable( false )
		.AddKeyValuePair( "rotation", transform.rotation, false, true )
		.EndTable();
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer,
											  const SpriteComponent& sprite )
{
	serializer.StartNewTable( "sprite" )
		.AddKeyValuePair( "width", sprite.width, true )
		.AddKeyValuePair( "height", sprite.height, false )
		.AddKeyValuePair( "startX", sprite.start_x, false )
		.AddKeyValuePair( "startY", sprite.start_y, false )
		.AddKeyValuePair( "layer", sprite.layer, false )
		.AddKeyValuePair( "sTexture", sprite.sTextureName, true, false, false, true )
		.StartNewTable( "uvs" )
		.AddKeyValuePair( "u", sprite.uvs.u, false )
		.AddKeyValuePair( "v", sprite.uvs.v, false )
		.AddKeyValuePair( "uv_width", sprite.uvs.uv_width, false )
		.AddKeyValuePair( "uv_height", sprite.uvs.uv_height, false, true )
		.EndTable( false )
		.StartNewTable( "color" )
		.AddKeyValuePair( "r", static_cast<int>( sprite.color.r ), false )
		.AddKeyValuePair( "g", static_cast<int>( sprite.color.g ), false )
		.AddKeyValuePair( "b", static_cast<int>( sprite.color.b ), false )
		.AddKeyValuePair( "a", static_cast<int>( sprite.color.a ), false, true )
		.EndTable( false )
		.AddKeyValuePair( "bHidden", sprite.bHidden, true )
		.AddKeyValuePair( "bIsoMetric", sprite.bIsoMetric, false )
		.AddKeyValuePair( "isoCellX", sprite.isoCellX, false )
		.AddKeyValuePair( "isoCellY", sprite.isoCellY, false, true )
		.EndTable();
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer,
											  const AnimationComponent& animation )
{
	serializer.StartNewTable( "animation" )
		.AddKeyValuePair( "numFrames", animation.numFrames, false )
		.AddKeyValuePair( "frameRate", animation.frameRate, false )
		.AddKeyValuePair( "frameOffset", animation.frameOffset, false )
		.AddKeyValuePair( "bVertical", animation.bVertical, false )
		.AddKeyValuePair( "bLooped", animation.bLooped, false, true )
		.EndTable( false );
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer,
											  const BoxColliderComponent& boxCollider )
{
	serializer.StartNewTable( "boxCollider" )
		.AddKeyValuePair( "width", boxCollider.width, false )
		.AddKeyValuePair( "height", boxCollider.height, false )
		.StartNewTable( "offset", false )
		.AddKeyValuePair( "x", boxCollider.offset.x, false )
		.AddKeyValuePair( "y", boxCollider.offset.y, false, true )
		.EndTable( false )
		.EndTable( false );
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer,
											  const CircleColliderComponent& circleCollider )
{
	serializer.StartNewTable( "circleCollider" )
		.AddKeyValuePair( "radius", circleCollider.radius, false )
		.StartNewTable( "offset", false )
		.AddKeyValuePair( "x", circleCollider.offset.x, false )
		.AddKeyValuePair( "y", circleCollider.offset.y, false, true )
		.EndTable( false )
		.EndTable( false );
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const TextComponent& text )
{
	serializer
		.StartNewTable( "text" ) // Start text table
		.AddKeyValuePair( "text", text.sTextStr, true, false, false, true )
		.AddKeyValuePair( "fontName", text.sFontName, true, false, false, true )
		.AddKeyValuePair( "padding", text.padding )
		.AddKeyValuePair( "wrap", text.wrap )
		.AddKeyValuePair( "bHidden", text.bHidden )
		.StartNewTable( "color" ) // Start Color table
		.AddKeyValuePair( "r", static_cast<int>( text.color.r ) )
		.AddKeyValuePair( "g", static_cast<int>( text.color.g ) )
		.AddKeyValuePair( "b", static_cast<int>( text.color.b ) )
		.AddKeyValuePair( "a", static_cast<int>( text.color.a ) )
		.EndTable();	   // End Color table
	serializer.EndTable(); // End Text table
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer,
											  const PhysicsComponent& physics )
{
	const auto& attributes = physics.GetAttributes();

	serializer.StartNewTable( "physics" )
		.StartNewTable( "attributes" )
		.AddKeyValuePair( "type", GetRigidBodyTypeString( attributes.eType ), false, false, false, true )
		.AddKeyValuePair( "density", attributes.density, false )
		.AddKeyValuePair( "friction", attributes.friction, false )
		.AddKeyValuePair( "restitution", attributes.restitution, false )
		.AddKeyValuePair( "restitutionThreshold", attributes.restitutionThreshold, false )
		.AddKeyValuePair( "radius", attributes.radius, false )
		.AddKeyValuePair( "gravityScale", attributes.gravityScale, true )
		.StartNewTable( "position" )
		.AddKeyValuePair( "x", attributes.position.x, false )
		.AddKeyValuePair( "y", attributes.position.y, false )
		.EndTable( false )
		.StartNewTable( "scale" )
		.AddKeyValuePair( "x", attributes.scale.x, false )
		.AddKeyValuePair( "y", attributes.scale.y, false, true )
		.EndTable( false )
		.StartNewTable( "boxSize" )
		.AddKeyValuePair( "x", attributes.boxSize.x, false )
		.AddKeyValuePair( "y", attributes.boxSize.y, false, true )
		.EndTable( false )
		.StartNewTable( "offset" )
		.AddKeyValuePair( "x", attributes.offset.x, false )
		.AddKeyValuePair( "y", attributes.offset.y, false, true )
		.EndTable( false )
		.AddKeyValuePair( "bCircle", attributes.bCircle, false )
		.AddKeyValuePair( "bBoxShape", attributes.bBoxShape, false )
		.AddKeyValuePair( "bFixedRotation", attributes.bFixedRotation, false )
		.AddKeyValuePair( "bIsSensor", attributes.bIsSensor, false )
		.AddKeyValuePair( "filterCategory", static_cast<unsigned>( attributes.filterCategory, false ) )
		.AddKeyValuePair( "filterMask", static_cast<unsigned>( attributes.filterMask, false ) )
		.AddKeyValuePair( "groupIndex", static_cast<int>( attributes.groupIndex, false ) )
		.StartNewTable( "objectData" )
		.AddKeyValuePair( "tag", attributes.objectData.tag, false, false, false, true )
		.AddKeyValuePair( "group", attributes.objectData.group, false, false, false, true )
		.AddKeyValuePair( "bCollider", attributes.objectData.bCollider, false )
		.AddKeyValuePair( "bTrigger", attributes.objectData.bTrigger, false )
		.AddKeyValuePair( "bIsFriendly", attributes.objectData.bIsFriendly, false, true )
		.EndTable( false )
		.EndTable()
		.EndTable();
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer,
											  const RigidBodyComponent& rigidBody )
{
	serializer
		.StartNewTable( "rigidBody" )	// Start rigidbody table
		.StartNewTable( "maxVelocity" ) // Start velocity table
		.AddKeyValuePair( "x", rigidBody.maxVelocity.x )
		.AddKeyValuePair( "y", rigidBody.maxVelocity.y )
		.EndTable()	 // end velocity table
		.EndTable(); // End rigid body table
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const Identification& id )
{
	serializer
		.StartNewTable( "id" ) // Start id table
		.AddKeyValuePair( "name", id.name, true, false, false, true )
		.AddKeyValuePair( "group", id.group, true, true, false, true )
		.EndTable(); // End id table
}

void ComponentSerializer::SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const UIComponent& ui )
{
	
}

void ComponentSerializer::DeserializeComponent( const sol::table& table, TransformComponent& transform )
{
	transform.position =
		glm::vec2{ table[ "position" ][ "x" ].get_or( 0.f ), table[ "position" ][ "y" ].get_or( 0.f ) };
	transform.scale = glm::vec2{ table[ "scale" ][ "x" ].get_or( 0.f ), table[ "scale" ][ "y" ].get_or( 0.f ) };
	transform.rotation = table[ "rotation" ].get_or( 0.f );
}

void ComponentSerializer::DeserializeComponent( const sol::table& table, SpriteComponent& sprite )
{
	sprite.width = table[ "width" ].get_or( 0.f );
	sprite.height = table[ "height" ].get_or( 0.f );

	sprite.uvs = UVs{ .u = table[ "uvs" ][ "u" ].get_or( 0.f ),
					  .v = table[ "uvs" ][ "v" ].get_or( 0.f ),
					  .uv_width = table[ "uvs" ][ "uv_width" ].get_or( 0.f ),
					  .uv_height = table[ "uvs" ][ "uv_height" ].get_or( 0.f ) };

	sprite.start_x = table[ "startX" ].get_or( 0 );
	sprite.start_y = table[ "startY" ].get_or( 0 );
	sprite.layer = table[ "layer" ].get_or( 0 );
	sprite.bHidden = table[ "bHidden" ].get_or( false );
	sprite.sTextureName = table[ "sTexture" ].get_or( std::string{ "" } );

	// Check if sprite should be isometic
	if ( table[ "bIsoMetric" ].valid() )
	{
		sprite.bIsoMetric = table[ "bIsoMetric" ].get_or( false );
		if ( table[ "isoCellX" ].valid() )
		{
			sprite.isoCellX = table[ "isoCellX" ].get_or( 0 );
		}
		if ( table[ "isoCellY" ].valid() )
		{
			sprite.isoCellY = table[ "isoCellY" ].get_or( 0 );
		}
	}
}

void ComponentSerializer::DeserializeComponent( const sol::table& table, AnimationComponent& animation )
{
	animation.numFrames = table[ "numFrames" ].get_or( 0 );
	animation.frameRate = table[ "frameRate" ].get_or( 0 );
	animation.frameOffset = table[ "frameOffset" ].get_or( 0 );
	animation.bVertical = table[ "bVertical" ].get_or( false );
	animation.bLooped = table[ "bLooped" ].get_or( false );
}

void ComponentSerializer::DeserializeComponent( const sol::table& table, BoxColliderComponent& boxCollider )
{
	boxCollider.width = table[ "width" ].get_or( 0 );
	boxCollider.height = table[ "height" ].get_or( 0 );
	boxCollider.offset = glm::vec2{ table[ "offset" ][ "x" ].get_or( 0.f ), table[ "offset" ][ "y" ].get_or( 0.f ) };
}

void ComponentSerializer::DeserializeComponent( const sol::table& table, CircleColliderComponent& circleCollider )
{
	circleCollider.radius = table[ "radius" ].get_or( 0.f );
	circleCollider.offset = glm::vec2{ table[ "offset" ][ "x" ].get_or( 0.f ), table[ "offset" ][ "y" ].get_or( 0.f ) };
}

void ComponentSerializer::DeserializeComponent( const sol::table& table, TextComponent& text )
{
	text.sTextStr = table[ "text" ].get_or( std::string{ "" } );
	text.sFontName = table[ "fontName" ].get_or( std::string{ "" } );
	text.padding = table[ "padding" ].get_or( 0 );
	text.wrap = table[ "wrap" ].get_or( 0 );
	text.bHidden = table[ "bHidden" ].get_or( false );

	text.color = SCION_RENDERING::Color{ .r = static_cast<GLubyte>( table[ "color" ][ "r" ].get_or( 255U ) ),
										 .g = static_cast<GLubyte>( table[ "color" ][ "g" ].get_or( 255U ) ),
										 .b = static_cast<GLubyte>( table[ "color" ][ "b" ].get_or( 255U ) ),
										 .a = static_cast<GLubyte>( table[ "color" ][ "a" ].get_or( 255U ) ) };
}

void ComponentSerializer::DeserializeComponent( const sol::table& table, PhysicsComponent& physics )
{
	if ( table[ "attributes" ].valid() )
	{
		const sol::table attr = table[ "attributes" ];
		PhysicsAttributes attributes{
			.eType = GetRigidBodyTypeByString( attr[ "type" ].get_or( std::string{ "" } ) ),
			.density = attr[ "density" ].get_or( 0.f ),
			.friction = attr[ "friction" ].get_or( 0.f ),
			.restitution = attr[ "restitution" ].get_or( 0.f ),
			.restitutionThreshold = attr[ "restitutionThreshold" ].get_or( 0.f ),
			.radius = attr[ "radius" ].get_or( 0.f ),
			.gravityScale = attr[ "gravityScale" ].get_or( 1.f ),
			.position = glm::vec2{ attr[ "position" ][ "x" ].get_or( 0.f ), attr[ "position" ][ "y" ].get_or( 0.f ) },
			.scale = glm::vec2{ attr[ "scale" ][ "x" ].get_or( 1.f ), attr[ "scale" ][ "y" ].get_or( 1.f ) },
			.boxSize = glm::vec2{ attr[ "boxSize" ][ "x" ].get_or( 0.f ), attr[ "boxSize" ][ "y" ].get_or( 0.f ) },
			.offset = glm::vec2{ attr[ "offset" ][ "x" ].get_or( 0.f ), attr[ "offset" ][ "y" ].get_or( 0.f ) },
			.bCircle = attr[ "bCircle" ].get_or( false ),
			.bBoxShape = attr[ "bBoxShape" ].get_or( false ),
			.bFixedRotation = attr[ "bFixedRotation" ].get_or( false ),
			.bIsSensor = attr[ "bIsSensor" ].get_or( false ),
			.filterCategory = static_cast<uint16_t>( attr[ "filterCategory" ].get_or( 0U ) ),
			.filterMask = static_cast<uint16_t>( attr[ "filterMask" ].get_or( 0U ) ),
			.groupIndex = static_cast<int16_t>( attr[ "groupIndex" ].get_or( 0U ) ),
			.objectData = ObjectData{ attr[ "objectData" ][ "tag" ].get_or( std::string{ "" } ),
									  attr[ "objectData" ][ "group" ].get_or( std::string{ "" } ),
									  attr[ "objectData" ][ "bCollider" ].get_or( false ),
									  attr[ "objectData" ][ "bTrigger" ].get_or( false ),
									  attr[ "objectData" ][ "bIsFriendly" ].get_or( false ) } };

		physics.GetChangableAttributes() = attributes;
	}
}

void ComponentSerializer::DeserializeComponent( const sol::table& table, RigidBodyComponent& rigidBody )
{
	rigidBody.maxVelocity.x = table[ "maxVelocity" ][ "x" ].get_or( 0.f );
	rigidBody.maxVelocity.y = table[ "maxVelocity" ][ "y" ].get_or( 0.f );
}

void ComponentSerializer::DeserializeComponent( const sol::table& table, Identification& id )
{
	id.name = table[ "name" ].get_or( std::string{ "" } );
	id.group = table[ "group" ].get_or( std::string{ "" } );
}

void ComponentSerializer::DeserializeComponent( const sol::table& table, UIComponent& ui )
{
	
}


} // namespace SCION_CORE::ECS
