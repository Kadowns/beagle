//
// Created by Ricardo on 4/24/2021.
//

#include <iostream>
#include <android_native_app_glue.h>
#include <jni.h>
#include <android/log.h>

#include <eagle/platform/android/android_application.h>
#include <beagle/engine.h>
#include <template/template_game.h>

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "threaded_app", __VA_ARGS__))

extern "C" {
void android_main(struct android_app *pApp) {
    try {
        eagle::AndroidApplication application(pApp, new beagle::Engine(new TemplateGame()));
        application.run();
    }
    catch (std::exception& e){
        LOGE("exception caught!");
    }
}
}