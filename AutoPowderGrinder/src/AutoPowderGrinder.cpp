#include "AutoPowderGrinder.h"

using namespace apg;
using namespace std::literals::chrono_literals;

AutoPowderGrinder::AutoPowderGrinder()
{
	std::cout << "=================================================== (Start of initialization)\n";

	this->initialized = this->initialize();

	std::cout << "=================================================== (Client status)\n";
	if (!this->initialized)
		std::cout << "[-] Status: not initialized\n";
	else
		std::cout << "[+] Status: initialized\n";
	std::cout << "=================================================== (End of initialization)\n\n\n";
}

bool AutoPowderGrinder::initialize()
{
	this->minecraft = std::make_shared<Minecraft>();
	if (!this->minecraft->isInitialized())
	{
		std::cout << "[-] An error occured while initializing Minecraft\n";
		return false;
	}
	else
	{
		Block::minecraft = this->minecraft;
	}

	this->blockManager = std::make_unique<BlockManager>(minecraft);
	if (!this->blockManager->isInitialized())
	{
		std::cout << "[-] An error occured while initializing BlockManager\n";
		return false;
	}

	this->pathfinder = std::make_unique<Pathfinder>(minecraft);
	if (!this->pathfinder->isInitialized())
	{
		std::cout << "[-] An error occured while initializing Pathfinder\n";
		return false;
	}

	return true;
}

jclass apg::getClass(JNIEnv* env, const std::string& path)
{
	static std::unordered_map<std::string, jclass> classMap;

	if (classMap.empty())
	{
		JavaVM* jvm = nullptr;
		env->GetJavaVM(&jvm);
		jvmtiEnv* tienv = nullptr;
		jvm->GetEnv((void**)&tienv, JVMTI_VERSION_1_2);
		jint classCount = 0;
		jclass* classes = nullptr;
		tienv->GetLoadedClasses(&classCount, &classes);
		jclass ClassClass = env->FindClass("java/lang/Class");
		jmethodID getName = env->GetMethodID(ClassClass, "getName", "()Ljava/lang/String;");

		for (int i = 0; i < classCount; ++i)
		{
			jstring name = (jstring)env->CallObjectMethod(classes[i], getName);
			const char* chars = env->GetStringUTFChars(name, nullptr);
			jsize size = env->GetStringUTFLength(name);
			std::string className(chars, size);
			env->ReleaseStringUTFChars(name, chars);
			for (char& character : className)
			{
				if (character == '.') character = '/';
			}
			classMap.insert({ className, classes[i] });
		}
		tienv->Deallocate((unsigned char*)classes);
	}

	jclass foundclass = nullptr;
	try
	{
		foundclass = classMap.at(path);
	}
	catch (...)
	{
		std::cerr << "[-] Failed to find class " + path << std::endl;
	}
	return foundclass;
}

void AutoPowderGrinder::run()
{
	if (!this->initialized)
	{
		std::cout << "[-] Not initialized; cannot run\n";

		std::this_thread::sleep_for(1s);
		return;
	}

	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		std::cout << "Nothing here yet\n";

		std::this_thread::sleep_for(1s);
	}
}

double apg::clampAngle(double angle, double min, double max)
{
	if (angle >= 90)
		angle -= 360;

	return std::clamp<double>(angle, min, max);
}