#include "jav8.h"

#include <cstring>
#include <iostream>
#include <memory>

#include <v8.h>

#include "Utils.h"

void JNICALL Java_lu_flier_script_V8ScriptEngineFactory_initialize(JNIEnv *, jclass)
{
  v8::V8::Initialize(); 
}

jobject JNICALL Java_lu_flier_script_V8ScriptEngineFactory_getParameter(JNIEnv *pEnv, jobject pObj, jstring key)
{
  jni::Env env(pEnv);

  const std::string name = env.GetString(key);

  if (name == "javax.script.name")  
    return pEnv->NewStringUTF("Jav8");
  
  if (name == "javax.script.engine")
    return pEnv->NewStringUTF("Google V8");
  if (name == "javax.script.engine_version")
    return pEnv->NewStringUTF(v8::V8::GetVersion());

  if (name == "javax.script.language")
    return pEnv->NewStringUTF("ECMAScript");
  if (name == "javax.script.language_version")
    return pEnv->NewStringUTF("1.8.5");

  env.Throw("java/lang/IllegalArgumentException", "Invalid key");
}

jobject JNICALL Java_lu_flier_script_V8CompiledScript_internalExecute
  (JNIEnv *pEnv, jobject pObj, jlong pCompiledScript, jobject pContext)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Script> compiledScript((v8::Script *) pCompiledScript);

  v8::Handle<v8::Value> result = compiledScript->Run();

  return env.HasCaught() ? NULL : env.Wrap(result);
}

jlong JNICALL Java_lu_flier_script_V8CompiledScript_internalCompile
  (JNIEnv *pEnv, jobject pObj, jstring pScript)
{
  jni::V8Env env(pEnv);

  if (!v8::Context::InContext())
  {
    env.Throw("java/lang/IllegalStateException", "Not in context");
  }

  const std::string script = env.GetString(pScript);

  v8::Handle<v8::Script> compiledScript = v8::Script::Compile(v8::String::New(script.c_str(), script.size()));

  return env.HasCaught() ? 0 : (jlong) *v8::Persistent<v8::Script>::New(compiledScript);
}

void JNICALL Java_lu_flier_script_V8CompiledScript_internalRelease
  (JNIEnv *pEnv, jobject pObj, jlong ptr)
{
  jni::V8Env env(pEnv);

  if (ptr) { v8::Persistent<v8::Script>((v8::Script *) ptr).Dispose(); }
}

jobject JNICALL Java_lu_flier_script_V8Context_getEntered(JNIEnv *pEnv, jclass)
{
  if (!v8::Context::InContext()) return NULL;

  jni::V8Env env(pEnv);

  return env.NewV8Context(v8::Context::GetEntered());
}

jobject JNICALL Java_lu_flier_script_V8Context_getCurrent(JNIEnv *pEnv, jclass)
{
  if (!v8::Context::InContext()) return NULL;

  jni::V8Env env(pEnv);

  return env.NewV8Context(v8::Context::GetCurrent());
}

jobject JNICALL Java_lu_flier_script_V8Context_getCalling(JNIEnv *pEnv, jclass)
{
  if (!v8::Context::InContext()) return NULL;

  jni::V8Env env(pEnv);

  return env.NewV8Context(v8::Context::GetCalling());
}

jboolean JNICALL Java_lu_flier_script_V8Context_inContext(JNIEnv *pEnv, jclass)
{
  jni::V8Env env(pEnv);

  return v8::Context::InContext() ? JNI_TRUE : JNI_FALSE;
}

jlong JNICALL Java_lu_flier_script_V8Context_internalCreate(JNIEnv *pEnv, jclass)
{
  jni::V8Env env(pEnv);

  return (jlong) *v8::Persistent<v8::Context>::New(v8::Context::New());
}

void JNICALL Java_lu_flier_script_V8Context_internalRelease
  (JNIEnv *pEnv, jobject, jlong ptr)
{
  jni::V8Env env(pEnv);

  if (ptr) { v8::Persistent<v8::Context>((v8::Context *) ptr).Dispose(); }
}

void JNICALL Java_lu_flier_script_V8Context_internalEnter
  (JNIEnv *pEnv, jobject, jlong ptr)
{
  jni::V8Env env(pEnv);

  if (ptr) { v8::Persistent<v8::Context>((v8::Context *) ptr)->Enter(); }
}

void JNICALL Java_lu_flier_script_V8Context_internalLeave
  (JNIEnv *pEnv, jobject, jlong ptr)
{
  jni::V8Env env(pEnv);

  if (ptr) { v8::Persistent<v8::Context>((v8::Context *) ptr)->Exit(); }
}

jobject JNICALL Java_lu_flier_script_V8Context_internalGetGlobal
  (JNIEnv *pEnv, jobject, jlong ptr)
{
  jni::V8Env env(pEnv);
  
  v8::Handle<v8::Object> global = v8::Persistent<v8::Context>((v8::Context *) ptr)->Global();

  return env.HasCaught() ? NULL : env.NewV8Object(global);
}

void JNICALL Java_lu_flier_script_V8Object_internalRelease
  (JNIEnv *pEnv, jobject pObj, jlong ptr)
{
  jni::V8Env env(pEnv);

  if (ptr) { v8::Persistent<v8::Object>((v8::Object *) ptr).Dispose(); }
}

jint JNICALL Java_lu_flier_script_V8Object_size(JNIEnv *pEnv, jobject pObj)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));

  return obj->GetPropertyNames()->Length();
}

jboolean JNICALL Java_lu_flier_script_V8Object_isEmpty(JNIEnv *pEnv, jobject pObj)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));

  return obj->GetPropertyNames()->Length() == 0;
}

void JNICALL Java_lu_flier_script_V8Object_clear(JNIEnv *pEnv, jobject pObj)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));

  v8::Handle<v8::Array> names = obj->GetPropertyNames();

  for (size_t i=0; i<names->Length(); i++)
  {
    obj->Delete(v8::Handle<v8::String>::Cast(names->Get(i)));   
  }
}

jboolean JNICALL Java_lu_flier_script_V8Object_containsKey
  (JNIEnv *pEnv, jobject pObj, jobject pKey)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));
  const std::string key = env.GetString((jstring) pKey); 

  return obj->Has(v8::String::New(key.c_str(), key.size())) ? JNI_TRUE : JNI_FALSE;
}

jobject JNICALL Java_lu_flier_script_V8Object_get
  (JNIEnv *pEnv, jobject pObj, jobject pKey)
{
  jni::V8Env env(pEnv);
  
  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));
  const std::string key = env.GetString((jstring) pKey);  

  v8::Handle<v8::Value> value = obj->Get(v8::String::New(key.c_str(), key.size()));

  return env.HasCaught() ? NULL : env.Wrap(value);
}

jobject JNICALL Java_lu_flier_script_V8Object_put
  (JNIEnv *pEnv, jobject pObj, jstring pKey, jobject pValue)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));
  const std::string key = env.GetString((jstring) pKey);  
  v8::Handle<v8::String> name = v8::String::New(key.c_str(), key.size());

  v8::Handle<v8::Value> value = obj->Get(name);
  obj->Set(name, env.Wrap(pValue));

  return env.HasCaught() ? NULL : env.Wrap(value);
}

jobject JNICALL Java_lu_flier_script_V8Object_remove
  (JNIEnv *pEnv, jobject pObj, jobject pKey)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));
  const std::string key = env.GetString((jstring) pKey);  
  v8::Handle<v8::String> name = v8::String::New(key.c_str(), key.size());

  v8::Handle<v8::Value> value = obj->Get(name);
  obj->Delete(name);

  return env.HasCaught() ? NULL : env.Wrap(value);
}

jobjectArray JNICALL Java_lu_flier_script_V8Object_internalGetKeys(JNIEnv *pEnv, jobject pObj)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));

  v8::Handle<v8::Array> names = obj->GetPropertyNames();
  jobjectArray keys = env.NewObjectArray("java/lang/String", names->Length());

  for (size_t i=0; i<names->Length(); i++)
  {
    v8::Handle<v8::String> name = v8::Handle<v8::String>::Cast(names->Get(i));

    pEnv->SetObjectArrayElement(keys, i, env.NewString(name));
  }

  return keys;
}

jobject JNICALL Java_lu_flier_script_V8Function_internalInvoke
  (JNIEnv *, jobject, jlong, jobject, jobjectArray)
{
  return NULL;
}