#include "saturn_textures.h"

#include <string>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <stdlib.h>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"

#include "src/saturn/saturn.h"
#include "src/saturn/saturn_models.h"

#include "data/dynos.cpp.h"
#include "pc/debuglog.h"
#include "pc/gfx/gfx_rendering_api.h"
#include "pc/gfx/gfx_pc.h"

std::vector<Expression> current_expressions;

static u8 *RGBA32_RGBA16(const u8 *aData, u64 aLength) {
    u8 *_Buffer = New<u8>(aLength);
    u8 *pBuffer = _Buffer;
    for (u64 i = 0; i != aLength; i += 4) {
        u8 _Red   = (aData[i + 0] * 31) / 255;
        u8 _Grn   = (aData[i + 1] * 31) / 255;
        u8 _Blu   = (aData[i + 2] * 31) / 255;
        //u8 _Alp   = (aData[i + 3] * 1) / 255;
        //u16 _Col  = (_Red << 11) | (_Grn << 6) | (_Blu << 1) | (_Alp << 0);
        u8 _Alp   = (aData[i + 3] > 128) ? 1 : 0;
        u16 _Col  = (_Red << 11) | (_Grn << 6) | (_Blu << 1) | _Alp;
        *(pBuffer++) = (_Col >> 8) & 0xFF;
        *(pBuffer++) = (_Col >> 0) & 0xFF;
    }
    return _Buffer;
}

/* Fetches a TexturePath's raw texture data */
u8* GetTextureData(TexturePath Texture, int* Width, int* Height) {
    if (Texture.RawData != 0) return Texture.RawData;

    std::cout << "Loading " << Texture.FilePath << std::endl;

    FILE *TextureFile = fopen(Texture.FilePath.c_str(), "rb");
    if (!TextureFile) return 0;

    fseek(TextureFile, 0, SEEK_END);
    TexData* TextureData = New<TexData>();
    TextureData->mPngData.Resize(ftell(TextureFile)); rewind(TextureFile);
    fread(TextureData->mPngData.begin(), sizeof(u8), TextureData->mPngData.Count(), TextureFile);
    fclose(TextureFile);

    u8 *RawData = stbi_load_from_memory(TextureData->mPngData.begin(), TextureData->mPngData.Count(), &TextureData->mRawWidth, &TextureData->mRawHeight, NULL, 4);
    std::cout << Texture.FilePath << " --> w" << TextureData->mRawWidth << " h" << TextureData->mRawHeight << " c-> " << TextureData->mPngData.Count() << std::endl;
    TextureData->mRawFormat = G_IM_FMT_RGBA;
    TextureData->mRawSize   = G_IM_SIZ_32b;
    TextureData->mRawData   = Array<u8>(RawData, RawData + (TextureData->mRawWidth * TextureData->mRawHeight * 4));
    free(RawData);

    u8 *_Buffer = TextureData->mRawData.begin(); /* RGBA-16 */ //RGBA32_RGBA16(TextureData->mRawData.begin(), TextureData->mRawData.Count());

    gfx_get_current_rendering_api()->select_texture(0, 0);
    gfx_get_current_rendering_api()->upload_texture(_Buffer, TextureData->mRawWidth, TextureData->mRawHeight);

    *Width = TextureData->mRawWidth;
    *Height = TextureData->mRawHeight;
    return _Buffer;
}

/* Loads textures into an Expression */
std::vector<TexturePath> LoadExpressionTextures(std::string FolderPath, Expression expression) {
    std::vector<TexturePath> textures;

    // Check if the expression's folder exists
    if (std::filesystem::is_directory(std::filesystem::path(FolderPath))) {
        for (const auto & entry : std::filesystem::recursive_directory_iterator(FolderPath)) {
            if (!std::filesystem::is_directory(entry.path())) {
                if (entry.path().extension() == ".png") {
                    // Only allow PNG files
                    TexturePath texture;
                    texture.FileName = entry.path().filename().generic_u8string();
                    texture.FilePath = entry.path().generic_u8string();
                    texture.RawData = GetTextureData(texture, &texture.Width, &texture.Height);
                    textures.push_back(texture);
                }
            }
        }
    }
    return textures;
}

/* Returns true if a model is missing an eyes folder */
bool UsingVanillaEyes(std::string modelFolderPath) {
    if (AnyModelsEnabled() &&
        std::filesystem::is_directory(modelFolderPath + "/expressions/eyes") ||
        std::filesystem::is_directory(modelFolderPath + "/expressions/eye"))
            return false;

    return true;
}

/* Loads an expression list into a specified model */
std::vector<Expression> LoadExpressions(std::string modelFolderPath) {
    std::vector<Expression> expressions_list;

    // Check if the model's /expressions folder exists
    if (std::filesystem::is_directory(std::filesystem::path(modelFolderPath + "/expressions"))) {
        for (const auto & entry : std::filesystem::directory_iterator(modelFolderPath + "/expressions")) {
            // Load individual expression folders
            if (std::filesystem::is_directory(entry.path())) {
                Expression expression;
                expression.FolderPath = entry.path().generic_u8string();
                expression.Name = entry.path().filename().generic_u8string();
                if (expression.Name == "eye")
                    expression.Name = "eyes";

                // Load all PNG files
                expression.Textures = LoadExpressionTextures(expression.FolderPath, expression);
                if (expression.Textures.size() <= 0) continue;

                // Eyes will always appear first
                if (expression.Name == "eyes") {
                    expressions_list.insert(expressions_list.begin(), expression);
                } else {
                    expressions_list.push_back(expression);
                }
            }
        }
    }

    // If no eyes folder was loaded, load from vanilla eyes
    if (UsingVanillaEyes(modelFolderPath))
        expressions_list.insert(expressions_list.begin(), LoadEyesFolder());

    return expressions_list;
}

/* Loads textures from dynos/eyes/ into a global Expression */
Expression LoadEyesFolder() {
    Expression VanillaEyes;
    // Check if the dynos/eyes/ folder exists
    if (std::filesystem::is_directory("dynos/eyes")) {
        VanillaEyes.Name = "eyes";
        VanillaEyes.FolderPath = "dynos/eyes";
        VanillaEyes.Textures = LoadExpressionTextures(VanillaEyes.FolderPath, VanillaEyes);
    }
    return VanillaEyes;
}

/* Handles texture replacement. Called from gfx_pc.c */
const void* saturn_bind_texture(const void* input, Object* currentObj) {
    if (input == nullptr) return input;
    const char* inputTexture = static_cast<const char*>(input);
    const char* outputTexture;
    std::string texName = inputTexture;

    if (texName.find("saturn_") != std::string::npos && currentObj == gMarioStates[0].marioObj) {
        for (int i = 0; i < current_expressions.size(); i++) {
            Expression expression = current_expressions[i];
            if (expression.CurrentIndex < 0) return input;
            // Checks if the incoming texture has the expression's "key"
            // This could be "saturn_eye", "saturn_mouth", etc.
            if (expression.PathHasReplaceKey(texName, "saturn_")) {
                current_expressions[i].Visible = true;

                if (texName.find_last_of("_") != std::string::npos)
                    current_expressions[i].Format = texName.substr(texName.find_last_of("_") + 1);
                return expression.Textures[expression.CurrentIndex].RawData;
            }
        }
    }

    return input;
}