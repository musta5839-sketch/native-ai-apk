#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include <android/asset_manager.h>

class AIEngine {
public:
    AIEngine();
    ~AIEngine();

    bool init(AAssetManager* assetManager, const char* modelPath);
    void shutdown();
    bool runInference(const float* input, float* output, int size);

private:
    void* impl;
};

#endif
