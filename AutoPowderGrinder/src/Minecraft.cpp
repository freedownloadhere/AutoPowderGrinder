#include "AutoPowderGrinder.h"

using namespace apg;

AutoPowderGrinder::Minecraft::Minecraft()
{
	this->initialized = this->initialize();

	if (!this->initialized)
		std::cout << "[-] An error occured while initializing Minecraft\n";
	else
		std::cout << "[+] Successfully initialized Minecraft\n";
}

bool AutoPowderGrinder::Minecraft::initialize()
{
	JavaVM* jvm{ nullptr };

	if (JNI_GetCreatedJavaVMs(&jvm, 1, nullptr) != 0)
	{
		std::cout << "	[-] Failed to get created Java VMs!\n";
		return false;
	}

	jvm->AttachCurrentThread((void**)&this->env, nullptr);
	if (this->env == nullptr)
	{
		std::cout << "	[-] Failed to attach current JVM thread!\n";
		return false;
	}

	this->mcClass = apg::getClass(this->env, "net/minecraft/client/Minecraft");
	if (this->mcClass == nullptr)
	{
		std::cout << "	[-] Failed to find class Minecraft!\n";
		return false;
	}

	jfieldID fieldID{ env->GetStaticFieldID(this->mcClass, "field_71432_P", "Lnet/minecraft/client/Minecraft;") };
	if (fieldID == nullptr)
	{
		std::cout << "	[-] Failed to get static field ID theMinecraft!\n";
		return false;
	}

	this->mcClassInstance = env->GetStaticObjectField(this->mcClass, fieldID);
	if (this->mcClassInstance == nullptr)
	{
		std::cout << "	[-] Failed to get static object field theMinecraft!\n";
		return false;
	}

	this->player = std::make_shared<Player>(
		env,
		mcClass,
		mcClassInstance
	);
	if (!this->player->isInitialized())
	{
		return false;
	}

	this->world = std::make_shared<World>(
		env,
		mcClass,
		mcClassInstance
	);
	if (!this->world->isInitialized())
	{
		return false;
	}

	this->chat = std::make_shared<Chat>(
		env,
		mcClass,
		mcClassInstance,
		this->player->getEntityPlayerSPClass(),
		this->player->getMcThePlayerInstance()
	);
	if (!this->chat->isInitialized())
	{
		return false;
	}

	return true;
}

bool AutoPowderGrinder::Minecraft::isInitialized()
{
	return this->initialized;
}