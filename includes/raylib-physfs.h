// raylib-physfs.h
// Rob Loach 2021

#ifndef RAYLIB_PHYSFS_H__
#define RAYLIB_PHYSFS_H__

#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

Image LoadImageFromPhysFS(const char* file);

#ifdef RAYLIB_PHYSFS_IMPLEMENTATION

Image LoadImageFromPhysFS(const char* file) {
    
}

#endif

#ifdef __cplusplus
}
#endif

#endif