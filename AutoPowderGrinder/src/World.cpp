#include "AutoPowderGrinder.h"

using namespace apg;

AutoPowderGrinder::Minecraft::World::World(
	JNIEnv* env,
	const jclass& mcClass,
	const jobject& mcClassInstance
)
{
	this->initialized = this->initialize(
		env, mcClass, mcClassInstance
	);

	if (!this->initialized)
		std::cout << "[-] An error occured while initializing World\n";
	else
		std::cout << "[+] Successfully initialized World\n";
}

bool AutoPowderGrinder::Minecraft::World::initialize(
	JNIEnv* env,
	const jclass& mcClass,
	const jobject& mcClassInstance
)
{
	this->env = env;

	this->worldClientClass = apg::getClass(this->env, "net/minecraft/client/multiplayer/WorldClient");
	if (worldClientClass == nullptr)
	{
		std::cout << "	[-] Could not find the world client class\n";
		return false;
	}

	this->blockPosClass = apg::getClass(this->env, "net/minecraft/util/BlockPos");
	if (blockPosClass == nullptr)
	{
		std::cout << "	[-] Could not find the block pos class\n";
		return false;
	}

	this->blockClass = apg::getClass(this->env, "net/minecraft/block/Block");
	if (blockClass == nullptr)
	{
		std::cout << "	[-] Could not find the block class\n";
		return false;
	}

	jfieldID worldFieldID = this->env->GetFieldID(mcClass, "field_71441_e", "Lnet/minecraft/client/multiplayer/WorldClient;");
	if (worldFieldID == nullptr)
	{
		std::cout << "	[-] Could not get theWorld field ID\n";
		return false;
	}

	this->worldInstance = this->env->GetObjectField(mcClassInstance, worldFieldID);
	if (worldInstance == nullptr)
	{
		std::cout << "	[-] Could not get world instance\n";
		return false;
	}

	this->getBlockState = this->env->GetMethodID(this->worldClientClass, "func_180495_p", "(Lnet/minecraft/util/BlockPos;)Lnet/minecraft/block/state/IBlockState;");
	if (this->getBlockState == nullptr)
	{
		std::cout << "	[-] Could not get block state method\n";
		return false;
	}

	this->blockPosConstructor = this->env->GetMethodID(this->blockPosClass, "<init>", "(DDD)V");
	if (this->blockPosConstructor == nullptr)
	{
		std::cout << "	[-] Could not get block pos constructor ID\n";
		return false;
	}

	jclass blockStateInterface = apg::getClass(this->env, "net/minecraft/block/state/IBlockState");
	if (blockStateInterface == nullptr)
	{
		std::cout << "	[-] Could not get the block state interface\n";
		return false;
	}

	this->getBlock = this->env->GetMethodID(blockStateInterface, "func_177230_c", "()Lnet/minecraft/block/Block;");
	if (this->getBlock == nullptr)
	{
		std::cout << "	[-] Could not get block method\n";
		return false;
	}

	this->getIDfromBlock = this->env->GetStaticMethodID(this->blockClass, "func_149682_b", "(Lnet/minecraft/block/Block;)I");
	if (this->getIDfromBlock == nullptr)
	{
		std::cout << "	[-] Could not get the getIDfromBlock static method\n";
		return false;
	}

	return true;
}

int AutoPowderGrinder::Minecraft::World::getBlockID(const Vector3& pos)
{
	if (!this->initialized)
	{
		std::cout << "The world object was not initialized properly\n";
		return -1;
	}

	jobject blockPosObj{ nullptr };	
	jobject blockState{ nullptr };	
	jobject block{ nullptr };		

	blockPosObj = this->env->NewObject(this->blockPosClass, this->blockPosConstructor, pos.x, pos.y, pos.z);
	if (blockPosObj == nullptr)
	{
		std::cout << "Could not create the block position object\n";
		return -1;
	}

	blockState = this->env->CallObjectMethod(this->worldInstance, getBlockState, blockPosObj);
	if (blockState == nullptr)
	{
		std::cout << "Could not get block state at " << pos.x << " " << pos.y << " " << pos.z << "\n";
		return -1;
	}

	block = this->env->CallObjectMethod(blockState, this->getBlock);
	if (block == nullptr)
	{
		std::cout << "Could not get the block at " << pos.x << " " << pos.y << " " << pos.z << "\n";
		return -1;
	}

	int blockID = this->env->CallStaticIntMethod(blockClass, getIDfromBlock, block);

	this->env->DeleteLocalRef(blockPosObj);

	return blockID;
}

bool AutoPowderGrinder::Minecraft::World::isInitialized()
{
	return this->initialized;
}