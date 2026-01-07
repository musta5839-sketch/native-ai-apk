#include "engine.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#define LOG_TAG "Engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

Engine::Engine() : window(nullptr), width(0), height(0), initialized(false), aiEngine(nullptr) {}

Engine::~Engine() {
    shutdown();
}

bool Engine::init(JNIEnv* env, jobject assetManager, const char* modelPath) {
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    if (!mgr) {
        LOGE("Failed to get asset manager");
        return false;
    }

    aiEngine = new AIEngine();
    if (!aiEngine->init(mgr, modelPath)) {
        LOGE("AI Engine initialization failed");
        delete aiEngine;
        aiEngine = nullptr;
        return false;
    }

    LOGI("Engine initialized successfully");
    initialized = true;
    return true;
}

void Engine::shutdown() {
    if (aiEngine) {
        aiEngine->shutdown();
        delete aiEngine;
        aiEngine = nullptr;
    }
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    initialized = false;
    LOGI("Engine shutdown");
}

void Engine::setWindow(ANativeWindow* newWindow) {
    if (window) {
        ANativeWindow_release(window);
    }
    window = newWindow;
    if (window) {
        ANativeWindow_acquire(window);
    }
}

void Engine::resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    LOGI("Surface resized to %dx%d", width, height);
}

void Engine::renderStep() {
    if (!initialized || !window || !aiEngine) return;

    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(window, &buffer, nullptr) < 0) {
        LOGE("Failed to lock window");
        return;
    }

    uint32_t* pixels = static_cast<uint32_t*>(buffer.bits);
    int stride = buffer.stride;

    // Basic clear with gradient
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * stride + x;
            uint8_t r = (x * 255) / width;
            uint8_t g = (y * 255) / height;
            uint8_t b = 128;
            pixels[idx] = (0xFF << 24) | (b << 16) | (g << 8) | r;
        }
    }

    // Run AI inference step
    float input[10] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    float output[10];
    if (aiEngine->runInference(input, output, 10)) {
        // Visualize inference result in center
        int centerX = width / 2;
        int centerY = height / 2;
        int radius = static_cast<int>(output[0] * 50);
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x*x + y*y <= radius*radius) {
                    int px = centerX + x;
                    int py = centerY + y;
                    if (px >= 0 && px < width && py >= 0 && py < height) {
                        pixels[py * stride + px] = 0xFFFFFFFF;
                    }
                }
            }
        }
    }

    ANativeWindow_unlockAndPost(window);
}
