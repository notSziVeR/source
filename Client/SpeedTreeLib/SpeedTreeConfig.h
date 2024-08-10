#pragma once
const int       c_nNumWindMatrices = 40;
const float     c_fNearLodFactor = 1000000000.0f;
const float     c_fFarLodFactor = 50000000000.0f;
const int c_nVertexShader_LeafLightingAdjustment = 70;
const int c_nVertexShader_Light = 71;
const int c_nVertexShader_Material = 74;
const int c_nVertexShader_TreePos = 52;
const int c_nVertexShader_CompoundMatrix = 0;
const int c_nVertexShader_WindMatrices = 54;
const int c_nVertexShader_LeafTables = 4;
const int c_nVertexShader_Fog = 85;
#define WRAPPER_USE_STATIC_LIGHTING
#if defined WRAPPER_USE_STATIC_LIGHTING && defined WRAPPER_USE_DYNAMIC_LIGHTING
#error Please define exactly one lighting mode
#endif
#define WRAPPER_USE_NO_WIND
#if defined WRAPPER_USE_GPU_WIND && defined WRAPPER_USE_CPU_WIND
#error Please define exactly one lighting mode
#elif defined WRAPPER_USE_GPU_WIND && defined WRAPPER_USE_NO_WIND
#error Please define exactly one lighting mode
#elif defined WRAPPER_USE_CPU_WIND && defined WRAPPER_USE_NO_WIND
#error Please define exactly one lighting mode
#endif
#define WRAPPER_USE_CPU_LEAF_PLACEMENT
#if defined WRAPPER_USE_GPU_LEAF_PLACEMENT && defined WRAPPER_USE_CPU_LEAF_PLACEMENT
#error Please define exactly one leaf placement algorithm
#endif
#define WRAPPER_FLIP_T_TEXCOORD
#define WRAPPER_FOREST_FROM_STF
#if defined WRAPPER_FOREST_FROM_STF && defined WRAPPER_FOREST_FROM_INSTANCES
#error Please define exactly one loading mechanism
#endif
#define WRAPPER_BILLBOARD_MODE
#define WRAPPER_USE_FOG