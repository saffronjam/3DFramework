#include "SaffronPCH.h"

#include <mono/jit/jit.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>

#include "Saffron/Platform/Windows/OSWindow.h"
#include "Saffron/Script/EntityScriptClass.h"
#include "Saffron/Script/MonoApi.h"

namespace Se
{
void MonoApi::Initialize(const String& name, const Filepath& assembliesFilepath)
{
	mono_set_assemblies_path(assembliesFilepath.string().c_str());
	mono_jit_init(name.c_str());
}

void MonoApi::Shutdown(MonoDomain* domain)
{
	mono_jit_cleanup(reinterpret_cast<::MonoDomain*>(domain));
}

MonoDomain* MonoApi::CreateDomain(const String& friendlyName)
{
	auto* friendlyAppDomainName = const_cast<char*>(friendlyName.c_str());
	return mono_domain_create_appdomain(friendlyAppDomainName, nullptr);
}

void MonoApi::UnloadDomain(MonoDomain* domain)
{
	mono_domain_unload(domain);
}

void MonoApi::SetActiveDomain(MonoDomain* domain, bool force)
{
	mono_domain_set(domain, force);
}

MonoAssembly* MonoApi::LoadAssembly(const Filepath& filepath)
{
	if (filepath.empty())
	{
		Log::Warn("Trying to load mono assembly with empty filepath");
		return nullptr;
	}
	const auto fpStr = filepath.string();

	const HANDLE file = CreateFileA(fpStr.c_str(), FILE_READ_ACCESS, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
	                                nullptr);
	if (file == INVALID_HANDLE_VALUE)
	{
		Log::Warn("Failed to load mono assembly: INVALID_HANDLE_VALUE");
		return nullptr;
	}

	const DWORD fileSize = GetFileSize(file, nullptr);
	if (fileSize == INVALID_FILE_SIZE)
	{
		Log::Warn("Failed to load mono assembly: INVALID_FILE_SIZE");
		CloseHandle(file);
		return nullptr;
	}

	void* fileData = malloc(fileSize);
	if (fileData == nullptr)
	{
		Log::Warn("Failed to allocate memory for mono assembly. Filesize: {0}", fileSize);
		CloseHandle(file);
		return nullptr;
	}

	DWORD read = 0;
	const bool readResult = ReadFile(file, fileData, fileSize, &read, nullptr);
	if (!readResult)
	{
		Log::Warn("Failed to read file for mono assembly");
		free(fileData);
		CloseHandle(file);
		return nullptr;
	}

	MonoImageOpenStatus status;
	MonoImage* image = mono_image_open_from_data_full(static_cast<char*>(fileData), fileSize, true, &status, false);
	if (status != MONO_IMAGE_OK)
	{
		Log::Warn("Failed to create image for mono assembly. MonoImageOpenStatus: {0}", status);
		return nullptr;
	}
	MonoAssembly* assemb = mono_assembly_load_from_full(image, fpStr.c_str(), &status, 0);
	free(fileData);
	CloseHandle(file);
	mono_image_close(image);

	Log::CoreInfo("Successfully loaded assembly: {0}", filepath.string());

	return assemb;
}

void MonoApi::AttachThread(MonoDomain* domain)
{
	mono_thread_attach(domain);
}

void MonoApi::DetachThread()
{
	mono_thread_detach(mono_thread_current());
}

uint MonoApi::Instantiate(MonoDomain* domain, EntityScriptClass& scriptClass)
{
	MonoObject* instance = mono_object_new(domain, scriptClass.Class);
	if (!instance) Log::CoreWarn("mono_object_new failed");

	mono_runtime_object_init(instance);
	const uint handle = mono_gchandle_new(instance, false);
	return handle;
}

MonoObject* MonoApi::CallMethod(MonoObject* object, MonoMethod* method, void** params)
{
	MonoObject* pException = nullptr;
	MonoObject* result = mono_runtime_invoke(method, object, params, &pException);
	return result;
}

MonoString* MonoApi::CreateString(MonoDomain* domain, const String& string)
{
	return mono_string_new(domain, string.c_str());
}

MonoImage* MonoApi::GetImage(MonoAssembly* assembly)
{
	MonoImage* image = mono_assembly_get_image(assembly);
	if (!image)
	{
		Log::CoreError("mono_assembly_get_image failed");
	}

	return image;
}

MonoClass* MonoApi::GetClass(MonoImage* image, const EntityScriptClass& scriptClass)
{
	return mono_class_from_name(image, scriptClass.NamespaceName.c_str(), scriptClass.ClassName.c_str());
}

MonoClass* MonoApi::GetClass(MonoImage* image, const String& namespaceName, const String& className)
{
	return mono_class_from_name(image, namespaceName.c_str(), className.c_str());
}

MonoClass* MonoApi::GetClass(MonoImage* image, const String& fullName)
{
	String namespaceName, className;
	if (fullName.find('.') != String::npos)
	{
		namespaceName = fullName.substr(0, fullName.find_last_of('.'));
		className = fullName.substr(fullName.find_last_of('.') + 1);
	}
	else
	{
		className = fullName;
		Log::CoreWarn("Getting mono class from full name that was not part of namespace: {}", fullName);
	}
	return GetClass(image, namespaceName, className);
}

bool MonoApi::IsSubclass(MonoClass* base, MonoClass* derived, bool includeInterface)
{
	return mono_class_is_subclass_of(base, derived, includeInterface);
}

MonoMethod* MonoApi::GetMethod(MonoImage* image, const String& methodDesc)
{
	MonoMethodDesc* desc = mono_method_desc_new(methodDesc.c_str(), NULL);
	if (!desc) Log::CoreWarn("mono_method_desc_new failed");

	MonoMethod* method = mono_method_desc_search_in_image(desc, image);
	if (!method) Log::CoreWarn("mono_method_desc_search_in_image failed");

	return method;
}

MonoMethod* MonoApi::GetGetter(MonoProperty* property)
{
	return mono_property_get_get_method(property);
}

MonoMethod* MonoApi::GetSetter(MonoProperty* property)
{
	return mono_property_get_set_method(property);
}

MonoProperty* MonoApi::GetProperty(MonoClass* monoClass, const String& name)
{
	return mono_class_get_property_from_name(monoClass, name.c_str());
}

MonoObject* MonoApi::GetInstance(uint handle)
{
	Debug::Assert(handle, "Invalid handle {}. Was entity not instantiated?", handle);;
	return mono_gchandle_get_target(handle);
}

MonoClassField* MonoApi::GetFields(MonoClass* monoClass, void** out)
{
	Debug::Assert(out != nullptr, "Output data address was null");
	return mono_class_get_fields(monoClass, out);
}

String MonoApi::GetFieldName(MonoClassField* field)
{
	return mono_field_get_name(field);
}

uint MonoApi::GetFieldFlags(MonoClassField* field)
{
	return mono_field_get_flags(field);
}

MonoType* MonoApi::GetFieldType(MonoClassField* field)
{
	return mono_field_get_type(field);
}

void MonoApi::GetFieldValue(MonoObject* object, MonoClassField* field, void* out)
{
	Debug::Assert(out != nullptr, "Output data address was null");
	mono_field_get_value(object, field, out);
}

void MonoApi::SetFieldValue(MonoObject* object, MonoClassField* field, void* value)
{
	Debug::Assert(value != nullptr, "Input data address was null");
	mono_field_set_value(object, field, value);
}

bool MonoApi::IsPublic(uint fieldFlag)
{
	return (fieldFlag & MONO_FIELD_ATTR_PUBLIC) != 0;
}

FieldType MonoApi::GetSaffronPublicFieldType(MonoType* monoType)
{
	const int type = mono_type_get_type(monoType);
	switch (type)
	{
	case MONO_TYPE_R4: return FieldType::Float;
	case MONO_TYPE_I4: return FieldType::Int;
	case MONO_TYPE_U4: return FieldType::Uint;
	case MONO_TYPE_STRING: return FieldType::String;
	case MONO_TYPE_VALUETYPE:
	{
		char* name = mono_type_get_name(monoType);
		if (strcmp(name, "Se.Vector2") == 0) return FieldType::Vec2;
		if (strcmp(name, "Se.Vector3") == 0) return FieldType::Vec3;
		return FieldType::Vec4;
	}
	default: return FieldType::None;
	}
}

void MonoApi::PrintClassMethods(MonoClass* monoClass)
{
	MonoMethod* iter;
	void* ptr = nullptr;
	while ((iter = mono_class_get_methods(monoClass, &ptr)) != nullptr)
	{
		printf("--------------------------------\n");
		const char* name = mono_method_get_name(iter);
		MonoMethodDesc* methodDesc = mono_method_desc_from_method(iter);

		const char* paramNames = "";
		mono_method_get_param_names(iter, &paramNames);

		Log::CoreInfo("Name: {0}", name);
		Log::CoreInfo("Full name: {0}", mono_method_full_name(iter, true));
	}
}

void MonoApi::PrintClassProperties(MonoClass* monoClass)
{
	MonoProperty* iter;
	void* ptr = nullptr;
	while ((iter = mono_class_get_properties(monoClass, &ptr)) != nullptr)
	{
		Log::CoreInfo("--------------------------------\n");
		Log::CoreInfo("Name: {0}", mono_property_get_name(iter));
	}
}
}
