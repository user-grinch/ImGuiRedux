#pragma once
#include "pch.h"

struct TextureInfo {
  public:
    std::string path;
    void *pTexture;
    unsigned int lastAccessed; // last time when the texture was accessed (seconds)

    TextureInfo() {};
    bool operator==(const TextureInfo &pInfo) {
        return (path == pInfo.path && pInfo.pTexture && pTexture == pInfo.pTexture);
    }
};

/*
    Handles loading, unloading images
*/
class TextureMgr {
  private:
    // static inline constexpr unsigned int TIMEOUT = 30; // Unload unused textures
    static inline std::vector<TextureInfo> textureList;

  public:
    TextureMgr() = delete;
    TextureMgr(TextureMgr&) = delete;

    // Returns true if texture exsits in list
    static bool Exists(TextureInfo *pInfo);

    // Unloads a texture from memory, handles different backends
    static void FreeTexture(TextureInfo *pInfo);

    // Returns TextureInfo from file path
    static TextureInfo* FindInfo(std::string&& path);

    // Loads texture, returns nullptr on failure
    static void LoadTexture(TextureInfo &info);

    // Loads texture from given path, returns nullptr on failure
    static TextureInfo* LoadTextureFromPath(const char *path);

    // Frees memory by clearing up unused textures
    // static void RunCleanup();
};