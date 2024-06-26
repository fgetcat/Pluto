#ifndef SaturnTextures
#define SaturnTextures

#include <stdio.h>
#include <stdbool.h>
#include <PR/ultratypes.h>

#ifdef __cplusplus
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <map>

class TexturePath {
public:
    std::string FileName;
    /* File name without extension */
    std::string ShortFileName() {
        size_t pos = FileName.find_last_of(".");
        if (pos > 0 && pos != std::string::npos)
            return FileName.substr(0, pos);
        return FileName;
    }
    std::string FilePath;
    u8 *RawData = 0;
    int Width;
    int Height;
    bool IsModelTexture = false;
};

class Expression {
public:
    std::string Name;
    std::string FolderPath;
    bool Enabled;
    std::vector<TexturePath> Textures;
    std::vector<TexturePath> Folders;
    /* The index of the current selected texture */
    int CurrentIndex = 0;

    /* Returns true if the replace key was detected in the texture pool
       (Note: This ignores whether or not an expression folder is present) */
    bool Visible;

    std::string Format;

    /* Returns true if a given path contains a prefix (saturn_) followed by the expression's replacement keywords
       For example, "/eye" can be "saturn_eye" or "saturn_eyes" */
    bool PathHasReplaceKey(std::string path, std::string prefix) {
        // We also append a _ or . to the end, so "saturn_eyebrow" doesn't get read as "saturn_eye"
        return (path.find(prefix + this->Name + "_") != std::string::npos ||
                path.find(prefix + this->Name + "s_") != std::string::npos ||
                path.find(prefix + this->Name.substr(0, this->Name.size() - 1) + "_") != std::string::npos);
    }

    /* Returns true if the expression's textures are formatted for a checkbox
       This is when an expression has two non-model textures and no subfolders */
    bool IsToggleFormat() {
        if (this->Textures.size() >= 2 && this->Folders.size() == 0) {
            int countWithoutModelTextures = 0;
            for (int i = 0; i < this->Textures.size(); i++) {
                // Exclude model textures
                if (this->Textures[i].IsModelTexture) continue;
                countWithoutModelTextures++;
            }
            return(countWithoutModelTextures == 2);
        }
        return false;
    }
};

extern u8* GetTextureData(TexturePath, int*, int*);

extern Expression LoadEyesFolder();
std::vector<TexturePath> LoadExpressionTextures(std::string, Expression);
std::vector<Expression> LoadExpressions(std::string);

extern std::vector<Expression> current_expressions;

extern "C" {
#endif    
    #include "include/types.h"
    const void* saturn_bind_texture(const void*, struct Object*);
#ifdef __cplusplus
}
#endif

#endif