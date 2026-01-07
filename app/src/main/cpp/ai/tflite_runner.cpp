#if ENABLE_TFLITE

#include "tflite_runner.h"
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/model.h>
#include <tensorflow/lite/kernels/register.h>
#include <android/asset_manager.h>
#include <android/log.h>

#define LOG_TAG "TFLiteRunner"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

TFLiteRunner::TFLiteRunner() : model(nullptr), interpreter(nullptr) {}

TFLiteRunner::~TFLiteRunner() {
    shutdown();
}

bool TFLiteRunner::init(AAssetManager* assetManager, const char* modelPath) {
    AAsset* asset = AAssetManager_open(assetManager, modelPath, AASSET_MODE_BUFFER);
    if (!asset) {
        LOGE("Failed to open model asset: %s", modelPath);
        return false;
    }

    const void* data = AAsset_getBuffer(asset);
    off_t length = AAsset_getLength(asset);

    model = TfLiteModelCreate(data, length);
    AAsset_close(asset);

    if (!model) {
        LOGE("Failed to create TFLite model");
        return false;
    }

    TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
    TfLiteInterpreterOptionsSetNumThreads(options, 2);

    interpreter = TfLiteInterpreterCreate(model, options);
    TfLiteInterpreterOptionsDelete(options);

    if (!interpreter) {
        LOGE("Failed to create interpreter");
        TfLiteModelDelete(model);
        model = nullptr;
        return false;
    }

    if (TfLiteInterpreterAllocateTensors(interpreter) != kTfLiteOk) {
        LOGE("Failed to allocate tensors");
        TfLiteInterpreterDelete(interpreter);
        TfLiteModelDelete(model);
        interpreter = nullptr;
        model = nullptr;
        return false;
    }

    LOGI("TFLite model loaded successfully");
    return true;
}

void TFLiteRunner::shutdown() {
    if (interpreter) {
        TfLiteInterpreterDelete(interpreter);
        interpreter = nullptr;
    }
    if (model) {
        TfLiteModelDelete(model);
        model = nullptr;
    }
}

bool TFLiteRunner::runInference(const float* input, float* output, int size) {
    if (!interpreter) return false;

    TfLiteTensor* input_tensor = TfLiteInterpreterGetInputTensor(interpreter, 0);
    if (TfLiteTensorByteSize(input_tensor) != size * sizeof(float)) {
        LOGE("Input size mismatch");
        return false;
    }

    if (TfLiteTensorCopyFromBuffer(input_tensor, input, size * sizeof(float)) != kTfLiteOk) {
        LOGE("Failed to copy input");
        return false;
    }

    if (TfLiteInterpreterInvoke(interpreter) != kTfLiteOk) {
        LOGE("Inference failed");
        return false;
    }

    const TfLiteTensor* output_tensor = TfLiteInterpreterGetOutputTensor(interpreter, 0);
    if (TfLiteTensorByteSize(output_tensor) != size * sizeof(float)) {
        LOGE("Output size mismatch");
        return false;
    }

    if (TfLiteTensorCopyToBuffer(output_tensor, output, size * sizeof(float)) != kTfLiteOk) {
        LOGE("Failed to copy output");
        return false;
    }

    return true;
}

#endif
