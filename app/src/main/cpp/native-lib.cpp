#include <jni.h>
#include <string>
#include "com_wzq_posixthread_PthreadUtil.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
static jmethodID gOnNativeMessage = NULL;
static JavaVM * gVm = NULL;
static jobject gObj = NULL;

static pthread_mutex_t mutex;

struct NativeWorkerArgs{
    jint id;
    jint iteratations;
};

jint JNI_OnLoad(JavaVM *vm, void * reserved)
{
    gVm = vm;
    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL Java_com_wzq_posixthread_PthreadUtil_nativeInit(JNIEnv *env, jobject obj){

    if(0 != pthread_mutex_init(&mutex, NULL))
    {
        jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(exceptionClazz, "Unable to initialize mutex");
        return;
    }

    if(NULL == gObj)
    {
        gObj = env->NewGlobalRef(obj);
        if(NULL == gObj)
        {
            return;
        }

        if(gOnNativeMessage == NULL)
        {
            jclass clazz = env->GetObjectClass(obj);
            gOnNativeMessage = env->GetMethodID(clazz, "onNativeMessage", "(Ljava/lang/String;)V");
            if(NULL == gOnNativeMessage)
            {
                jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
                env->ThrowNew(exceptionClazz, "Unable to find method");
            }
        }

    }
}


JNIEXPORT void JNICALL Java_com_wzq_posixthread_PthreadUtil_nativeFree(JNIEnv *env, jobject obj){
    if(0 != pthread_mutex_destroy(&mutex))
    {
        jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(exceptionClazz, "Unable to destory mutex");
    }
    if(NULL != gObj)
    {
        env->DeleteGlobalRef(gObj);
        gObj = NULL;
    }

 }


 static void* nativeWorkerThread(void *args)
 {
    JNIEnv *env = NULL;
    if(0 == gVm->AttachCurrentThread(&env, NULL))
    {
        NativeWorkerArgs *nativeWorkerArgs = (NativeWorkerArgs *) args;
        Java_com_wzq_posixthread_PthreadUtil_nativeWorker(env, gObj, nativeWorkerArgs->id, nativeWorkerArgs->iteratations);
        delete nativeWorkerArgs;
        gVm->DetachCurrentThread();
    }
    return (void*) 1;
 }


//
JNIEXPORT void JNICALL Java_com_wzq_posixthread_PthreadUtil_posixThreads(JNIEnv *env, jobject obj, jint threadIds, jint iteratations)
{
   pthread_t *handles = new pthread_t[threadIds];
     for(jint i = 0; i < threadIds; i ++)
     {
         NativeWorkerArgs *nativeWorkerArgs = new NativeWorkerArgs();
         nativeWorkerArgs->id = i;
         nativeWorkerArgs->iteratations = iteratations;

         int result = pthread_create(&handles[i], NULL, nativeWorkerThread, (void *) nativeWorkerArgs);
         if(0 != result)
         {
            jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(exceptionClazz, "Unable to create thread");
            return;
         }
     }


     for(jint i = 0; i < threadIds; i ++)
     {
        void *result = NULL;
        if(0 != pthread_join(handles[i], &result))
        {
            jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(exceptionClazz, "Unable to join thread");
        }
        else
        {
            char message[26];
            sprintf(message, "Worker %d: Iteration %d", i, result);
            jstring messageString = env->NewStringUTF(message);
            env->CallVoidMethod(obj, gOnNativeMessage, messageString);
            if(NULL != env->ExceptionOccurred())
            {
                return;
            }
        }
     }
}

 void  Java_com_wzq_posixthread_PthreadUtil_nativeWorker(JNIEnv *env, jobject obj, jint threadIds, jint iteratations){
       if(0 != pthread_mutex_lock(&mutex))
       {
            jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(exceptionClazz, "Unable to lock mutex");
            return;
       }


        for(jint i = 0; i < iteratations; i ++)
        {
            char message[26];
            sprintf(message, "Worker %d: Iteration %d", threadIds, i);
            jstring messageString = env->NewStringUTF(message);
            env->CallVoidMethod(obj, gOnNativeMessage, messageString);
            if(NULL != env->ExceptionOccurred())
            {
                break;
            }
           // sleep(1);
        }



       if(0 != pthread_mutex_unlock(&mutex))
       {
            jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(exceptionClazz, "Unable to unlock mutex");
       }
}

