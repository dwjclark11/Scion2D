#include "AssetManager.h"
#include <Rendering/Essentials/TextureLoader.h>
#include <Rendering/Essentials/ShaderLoader.h>
#include <Logger/Logger.h>

namespace SCION_RESOURCES {

    bool AssetManager::AddTexture(const std::string& textureName, const std::string& texturePath, bool pixelArt)
    {
        // Check to see if the texture already exists
        if (m_mapTextures.find(textureName) != m_mapTextures.end())
        {
            SCION_ERROR("Failed to add texture [{0}] -- Already exists!", textureName);
            return false;
        }

        auto texture = SCION_RENDERING::TextureLoader::Create(
            pixelArt ? SCION_RENDERING::Texture::TextureType::PIXEL : SCION_RENDERING::Texture::TextureType::BLENDED,
            texturePath
        );

        if (!texture)
        {
            SCION_ERROR("Failed to load texture [{0}] at path [{1}]", textureName, texturePath);
            return false;
        }

        m_mapTextures.emplace(textureName, std::move(texture));
        return true;
    }

    std::shared_ptr<SCION_RENDERING::Texture> AssetManager::GetTexture(const std::string& textureName)
    {
        auto texItr = m_mapTextures.find(textureName);
        if (texItr == m_mapTextures.end())
        {
            SCION_ERROR("Failed to get texture [{0}] -- Does not exist!", textureName);
            return nullptr;
        }

        return texItr->second;
    }

    bool AssetManager::AddShader(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath)
    {
        // Check to see if the shader already exists
        if (m_mapShader.find(shaderName) != m_mapShader.end())
        {
            SCION_ERROR("Failed to add shader [{0}] -- Already Exists!", shaderName);
            return false;
        }

        // Create and load the shader
        auto shader = std::move(SCION_RENDERING::ShaderLoader::Create(vertexPath, fragmentPath));

        if (!shader)
        {
            SCION_ERROR("Failed to load Shader [{0}] at vert path [{1}] and frag path [{2}]", shaderName, vertexPath, fragmentPath);
            return false;
        }

        m_mapShader.emplace(shaderName, std::move(shader));
        return true;
    }

    std::shared_ptr<SCION_RENDERING::Shader> AssetManager::GetShader(const std::string& shaderName)
    {
        auto shaderItr = m_mapShader.find(shaderName);
        if (shaderItr == m_mapShader.end())
        {
            SCION_ERROR("Failed to get shader [{0}] -- Does not exist!", shaderName);
            return nullptr;
        }

        return shaderItr->second;
    }

    void AssetManager::CreateLuaAssetManager(sol::state& lua, SCION_CORE::ECS::Registry& registry)
    {
        auto& asset_manager = registry.GetContext<std::shared_ptr<AssetManager>>();
        if (!asset_manager)
        {
            SCION_ERROR("Failed to bind the asset manager to lua - Does not exist in the registry!");
            return;
        }

        lua.new_usertype<AssetManager>(
            "AssetManager",
            sol::no_constructor,
            "add_texture", [&](const std::string& assetName, const std::string& filepath, bool pixel_art)
            {
                return asset_manager->AddTexture(assetName, filepath, pixel_art);
            }
        );
    }
}