#include "EnergyReading.h"
#include "readenergy.h" 
#include "jni.h"

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

    jclass tempLocalClassRef = (*env)->FindClass(env, "EnergyReading");
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
    Java_EnergyReading_measureEnergy(JNIEnv *env, jobject self)
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

