#include "common.h"

bool g_EnableDebugFeatures =
#ifdef GL_GFX_DEBUG_FEATURES_ENABLED
    true;
#else
    false;
#endif
