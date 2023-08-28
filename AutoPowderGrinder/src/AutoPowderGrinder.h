#pragma once

#include <memory>
#include <jni.h>
#include <string>
#include <thread>
#include <iostream>
#include <Windows.h>
#include <cstdio>

class AutoPowderGrinder
{
private:
	class Minecraft;
	class StoneMiner;
	class ChestOpener;
	class ItemManager;
	class Pathfinder;

	std::shared_ptr<Minecraft> minecraft;

	/*std::unique_ptr<StoneMiner> stoneMiner;
	std::unique_ptr<ChestOpener> chestOpener;
	std::unique_ptr<ItemManager> itemManager;
	std::unique_ptr<Pathfinder> pathfinder;*/

	bool initialized{ false };
	bool running{ false };

	bool initialize();

public:
	AutoPowderGrinder();

	void run();
};

class AutoPowderGrinder::Minecraft
{
public:
	class Player;
	class World;

	std::shared_ptr<Player> player;
	std::shared_ptr<World> world;

	Minecraft();
	bool isInitialized();

private:
	jclass 
		mcClass{ nullptr };
	jobject 
		mcClassInstance{ nullptr };

	bool initialized{ false };
	JNIEnv* env{ nullptr };
	bool initialize();
};

struct Position
{
	double x{ 0 }, y{ 0 }, z{ 0 };
};

class AutoPowderGrinder::Minecraft::Player
{
public:
	Player(
		JNIEnv* env,
		const jclass& mcClass,
		const jobject& mcClassInstance
	);

	bool isInitialized();
	void sendChatMessage(const std::string& message);
	void updateMainInventory();
	void updatePosition();
	std::string getItem(int index);
	std::string updateAndGetItem(int index);

private:
	Position position{ 0 };
	std::string inventory[36] = {};

	jclass 
		EntityPlayerSPClass{ nullptr },
		InventoryPlayerClass{ nullptr },
		itemStackClass{ nullptr },
		chatCompClass{ nullptr };
	jobject 
		mcThePlayerInstance{ nullptr },
		inventoryInstance{ nullptr };
	jobjectArray 
		mainInventoryArray{ nullptr };
	jmethodID 
		addChatMessage{ nullptr },
		messageConstructor{ nullptr },
		displayNameGetter{ nullptr };
	jfieldID
		positionX{ nullptr },
		positionY{ nullptr },
		positionZ{ nullptr };

	// TODO Blocks around player

	bool initialized{ false };
	JNIEnv* env{ nullptr };
	bool initialize(
		JNIEnv* env,
		const jclass& mcClass,
		const jobject& mcClassInstance
	);
};

class AutoPowderGrinder::Minecraft::World
{
public:
	World(
		JNIEnv* env,
		const jclass& mcClass,
		const jobject& mcClassInstance
	);

	bool isInitialized();
	int getBlockID(const Position& pos);

private:
	jclass 
		worldClientClass{ nullptr },
		blockPosClass{ nullptr },
		blockClass{ nullptr };
	jmethodID 
		getBlockState{ nullptr },
		getBlock{ nullptr },
		blockPosConstructor{ nullptr },
		getIDfromBlock{ nullptr };
	jobject 
		worldInstance{ nullptr };

	bool initialized{ false };
	JNIEnv* env{ nullptr };
	bool initialize(
		JNIEnv* env,
		const jclass& mcClass,
		const jobject& mcClassInstance
	);
};
