#ifndef SaturnModels
#define SaturnModels

#include <PR/ultratypes.h>
#include "saturn_textures.h"


#ifdef __cplusplus

#include <vector>
#include <string>

extern bool IsSaturnModel(int);
extern bool AnyModelsEnabled();
extern bool IsAllRGBA32(std::vector<Expression>);

extern "C" {
#endif    
    extern void LoadModelData(int, bool);
    extern void RefreshActiveExpressions();
#ifdef __cplusplus
}
#endif

#endif