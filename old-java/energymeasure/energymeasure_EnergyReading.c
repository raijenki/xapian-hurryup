#define _GNU_SOURCE
#include "energymeasure_EnergyReading.h"
#include "readenergy.h" 
#include "jni.h"
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static struct emeter emeter;

static jclass JC_EnergyReading;
static jfieldID JF_energySys;
static jfieldID JF_energyA57;
static jfieldID JF_energyA53;

static jint JNI_VERSION = JNI_VERSION_1_8;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if((*vm)->GetEnv(vm, (void**)(&env), JNI_VERSION) != JNI_OK) {
        return JNI_ERR;
    }

    jclass tempLocalClassRef = (*env)->FindClass(env, "energymeasure/EnergyReading");
    JC_EnergyReading = (jclass) (*env)->NewGlobalRef(env, tempLocalClassRef);
    (*env)->DeleteLocalRef(env, tempLocalClassRef);

    JF_energySys = (*env)->GetFieldID(env, JC_EnergyReading,
                                      "energySys", "D");
    JF_energyA57 = (*env)->GetFieldID(env, JC_EnergyReading,
                                      "energyA57", "D");
    JF_energyA53 = (*env)->GetFieldID(env, JC_EnergyReading,
                                      "energyA53", "D");

    emeter_init(&emeter, NULL);
    return JNI_VERSION;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    JNIEnv* env;
    (*vm)->GetEnv(vm, (void**)(&env), JNI_VERSION);
    (*env)->DeleteGlobalRef(env, JC_EnergyReading);
    emeter_finalize(&emeter);
}

JNIEXPORT void JNICALL
    Java_energymeasure_EnergyReading_measureEnergy(JNIEnv *env, jobject self)
{
    struct reading reading;
    emeter_read_measurements(&emeter, &reading);
    (*env)->SetDoubleField(env, self, JF_energySys,
                           reading.sys_enm_ch0_sys);
    (*env)->SetDoubleField(env, self, JF_energyA57,
                           reading.sys_enm_ch1_a57);
    (*env)->SetDoubleField(env, self, JF_energyA53,
                           reading.sys_enm_ch0_a53);
}

JNIEXPORT void JNICALL Java_energymeasure_EnergyReading_setAffinity
  (JNIEnv *env, jclass class, jint pid , jint affinity)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    for(int i = 0; i < 32; ++i)
    {
        if(affinity & (1 << i))
            CPU_SET(i, &mask);
    }

    if(sched_setaffinity(pid, sizeof(mask), &mask) != 0)
    {
        fprintf(stderr,"Unable to set thread %d affinity. %s\n", pid, strerror(errno));
        *(int*)(0) = 0;
    }
}

JNIEXPORT jint JNICALL Java_energymeasure_EnergyReading_getAffinity
  (JNIEnv *env, jclass class, jint pid)
{
    cpu_set_t mask;
    if(sched_getaffinity(pid, sizeof(mask), &mask) != 0) {
        fprintf(stderr,"Unable to get thread %d affinity. %s\n", pid, strerror(errno));
        *(int*)(0) = 0;
        return -1;
    }

    jint result = 0;
    for(int i = 0; i < 32; ++i)
    {
        if(CPU_ISSET(i, &mask))
            result |= (1 << i);
    }

    return result;
}
