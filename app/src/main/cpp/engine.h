#ifndef ENGINE_H
#define ENGINE_H

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "ai/ai_engine.h"

class Engine {
public:
    Engine();
    ~Engine();

    bool init(JNIEnv* env, jobject assetManager, const char* modelPath);
    void shutdown();
    void setWindow(ANativeWindow* window);
    void resize(int width, int height);
    void renderStep();

private:
    ANativeWindow* window;
    int width;
    int height;
    bool initialized;
    AIEngine* aiEngine;
};

#endif
