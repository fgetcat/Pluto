#ifndef SaturnAnimations
#define SaturnAnimations

#include <PR/ultratypes.h>
#include "include/types.h"
extern const char* saturn_animations[];

#ifdef __cplusplus
#include <string>
#include <vector>

class PlutoAnim {
    public:
        std::string Name;
        std::string Author;
        bool Looping = false;
        int Length = -1;
        int Nodes;
        std::vector<s16> Values;
        std::vector<u16> Indices;
        int BoneCount = 20;
};

extern PlutoAnim LoadPAnim(std::string filePath);
extern std::vector<std::string> GetPAnimList(std::string folderPath);
extern void saturn_play_pluto_animation();
extern bool saturn_check_for_chainer();

extern std::vector<std::string> pluto_animations_list;

extern s16 bone_anim_values[61];
extern u16 bone_anim_indices[126];
extern bool is_editing_panim;
extern bool enable_bone_editor;
extern Vec3f bone_rotations[20];

extern PlutoAnim current_pluto_anim;

#endif

#endif