#include "AssetManager.h"
#include <Rendering/Essentials/TextureLoader.h>
#include <Rendering/Essentials/ShaderLoader.h>
#include <Rendering/Essentials/FontLoader.h>
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

    bool AssetManager::AddFont(const std::string& fontName, const std::string& fontPath, float fontSize)
    {
        if (m_mapFonts.contains(fontName))
        {
            SCION_ERROR("Failed to add font [{0}] -- Already Exists!", fontName);
            return false;
        }

        auto pFont = SCION_RENDERING::FontLoader::Create(fontPath, fontSize);

        if (!pFont)
        {
            SCION_ERROR("Failed to add font [{}] at path [{}] -- to the asset manager!", fontName, fontPath);
            return false;
        }

        m_mapFonts.emplace(fontName, std::move(pFont));

        return true;
    }

    bool AssetManager::AddFontFromMemory(const std::string& fontName, unsigned char* fontData, float fontSize)
    {

        if (m_mapFonts.contains(fontName))
        {
            SCION_ERROR("Failed to add font [{0}] -- Already Exists!", fontName);
            return false;
        }

        auto pFont = SCION_RENDERING::FontLoader::CreateFromMemory(fontData, fontSize);

        if (!pFont)
        {
            SCION_ERROR("Failed to add font [{0}] from memory -- to the asset manager!", fontName);
            return false;
        }

        m_mapFonts.emplace(fontName, std::move(pFont));

        return true;
    }

    std::shared_ptr<SCION_RENDERING::Font> AssetManager::GetFont(const std::string& fontName)
    {
        auto fontItr = m_mapFonts.find(fontName);
        if ( fontItr == m_mapFonts.end())
        {
            SCION_ERROR("Failed to get font [{0}] -- Does not exist!", fontName);
            return nullptr;
        }

        return fontItr->second;
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

    bool AssetManager::AddShaderFromMemory(const std::string& shaderName, const char* vertexShader, const char* fragShader)
    {
        if (m_mapShader.find(shaderName) != m_mapShader.end())
        {
            SCION_ERROR("Failed to add shader - [{0}] -- Already exists!", shaderName);
            return false;
        }

        auto shader = std::move(SCION_RENDERING::ShaderLoader::CreateFromMemory(vertexShader, fragShader));
        m_mapShader.insert(std::make_pair(shaderName, std::move(shader)));

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

    bool AssetManager::AddMusic(const std::string& musicName, const std::string& filepath)
    {
        if (m_mapMusic.find(musicName) != m_mapMusic.end())
        {
            SCION_ERROR("Failed to add music [{}] -- Already exists!", musicName);
            return false;
        }

        Mix_Music* music = Mix_LoadMUS(filepath.c_str());

        if (!music)
        {
            std::string error{Mix_GetError()};
            SCION_ERROR("Failed to load [{}] at path [{}] -- Mixer Error: {}", musicName, filepath, error);
            return false;
        }

        // Create the sound parameters
        SCION_SOUNDS::SoundParams params {
                .name = musicName,
                .filename = filepath,
                .duration = Mix_MusicDuration(music)
        };

        // Create the music Pointer
        auto musicPtr = std::make_shared<SCION_SOUNDS::Music>(params, MusicPtr{ music });
        if (!musicPtr)
        {
            SCION_ERROR("Failed to create the music ptr for [{}]", musicName);
            return false;
        }

        m_mapMusic.emplace(musicName, std::move(musicPtr));

        return true;
    }

    std::shared_ptr<SCION_SOUNDS::Music> AssetManager::GetMusic(const std::string& musicName)
    {
        auto musicItr = m_mapMusic.find(musicName);
        if (musicItr == m_mapMusic.end())
        {
            SCION_ERROR("Failed to get [{}] -- Does not exist!", musicName);
            return nullptr;
        }

        return musicItr->second;
    }

    bool AssetManager::AddSoundFx(const std::string& soundFxName, const std::string& filepath)
    {
        if (m_mapSoundFx.find(soundFxName) != m_mapSoundFx.end())
        {
            SCION_ERROR("Failed to add soundfx [{}] -- Already exists!", soundFxName);
            return false;
        }

        Mix_Chunk* chunk = Mix_LoadWAV(filepath.c_str());

        if (!chunk)
        {
            std::string error{Mix_GetError()};
            SCION_ERROR("Failed to load [{}] at path [{}] -- Error: {}", soundFxName, filepath, error);
            return false;
        }

        SCION_SOUNDS::SoundParams params {
            .name = soundFxName,
            .filename = filepath,
            .duration = chunk->alen / 179.4
        };

        auto pSoundFx = std::make_shared<SCION_SOUNDS::SoundFX>(params, SoundFxPtr{ chunk });
        m_mapSoundFx.emplace(soundFxName, std::move(pSoundFx));

        return true;
    }

    std::shared_ptr<SCION_SOUNDS::SoundFX> AssetManager::GetSoundFx(const std::string& soundFxName)
    {
        auto soundItr = m_mapSoundFx.find(soundFxName);
        if (soundItr == m_mapSoundFx.end())
        {
            SCION_ERROR("Failed to get SoundFX [{}] -- Does Not exist!", soundFxName);
            return nullptr;
        }

        return soundItr->second;
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
            },
            "add_music", [&](const std::string& musicName, const std::string& filepath)
            {
                return asset_manager->AddMusic(musicName, filepath);
            },
            "add_soundfx", [&](const std::string& soundFxName, const std::string& filepath)
            {
                return asset_manager->AddSoundFx(soundFxName, filepath);
            },
            "add_font", [&](const std::string& fontName, const std::string& fontPath, float fontSize)
            {
                return asset_manager->AddFont(fontName, fontPath, fontSize);
            }
        );
    }
}