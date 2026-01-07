#include <jni.h>
#include <android/asset_manager_jni.h>
#include "engine.h"

static Engine* g_engine = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_native_ultimate_MainActivity_nativeInit(JNIEnv* env, jobject thiz) {
    if (g_engine) return;
    g_engine = new Engine();

    jobject assetManager = env->NewLocalRef(
        env->CallObjectMethod(thiz,
            env->GetMethodID(env->GetObjectClass(thiz), "getAssets", "()Landroid/content/res/AssetManager;")
        )
    );
    if (!assetManager) return;

    const char* modelPath = "models/game_ai.tflite";
    if (!g_engine->init(env, assetManager, modelPath)) {
        delete g_engine;
        g_engine = nullptr;
    }
    env->DeleteLocalRef(assetManager);
}

extern "C" JNIEXPORT void JNICALL
Java_com_native_ultimate_MainActivity_nativeShutdown(JNIEnv* env, jobject thiz) {
    if (g_engine) {
        g_engine->shutdown();
        delete g_engine;
        g_engine = nullptr;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_native_ultimate_MainActivity_nativeSurfaceCreated(JNIEnv* env, jobject thiz, jlong surface) {
    if (g_engine) {
        g_engine->setWindow(reinterpret_cast<ANativeWindow*>(surface));
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_native_ultimate_MainActivity_nativeSurfaceChanged(JNIEnv* env, jobject thiz, jint width, jint height) {
    if (g_engine) {
        g_engine->resize(width, height);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_native_ultimate_MainActivity_nativeSurfaceDestroyed(JNIEnv* env, jobject thiz) {
    if (g_engine) {
        g_engine->setWindow(nullptr);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_native_ultimate_MainActivity_nativeRenderStep(JNIEnv* env, jobject thiz) {
    if (g_engine) {
        g_engine->renderStep();
    }
}
