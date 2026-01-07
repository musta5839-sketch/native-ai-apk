#include "ai_engine.h"
#include <android/log.h>

#define LOG_TAG "AIEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#if ENABLE_TFLITE
#include "tflite_runner.h"
#else
class TFLiteRunner {
public:
    bool init(AAssetManager*, const char*) { return false; }
    void shutdown() {}
    bool runInference(const float*, float*, int) { return false; }
};
#endif

AIEngine::AIEngine() : impl(nullptr) {}

AIEngine::~AIEngine() {
    shutdown();
}

bool AIEngine::init(AAssetManager* assetManager, const char* modelPath) {
#if ENABLE_TFLITE
    auto runner = new TFLiteRunner();
    if (runner->init(assetManager, modelPath)) {
        impl = runner;
        LOGI("TFLite AI Engine initialized");
        return true;
    }
    delete runner;
#endif
    LOGI("AI Engine running in stub mode");
    return true; // Always succeed in stub mode
}

void AIEngine::shutdown() {
#if ENABLE_TFLITE
    if (impl) {
        static_cast<TFLiteRunner*>(impl)->shutdown();
        delete static_cast<TFLiteRunner*>(impl);
        impl = nullptr;
    }
#endif
}

bool AIEngine::runInference(const float* input, float* output, int size) {
#if ENABLE_TFLITE
    if (impl) {
        return static_cast<TFLiteRunner*>(impl)->runInference(input, output, size);
    }
#endif
    // Stub behavior: copy input to output with slight modification
    for (int i = 0; i < size; i++) {
        output[i] = input[i] * 0.5f;
    }
    return true;
}
