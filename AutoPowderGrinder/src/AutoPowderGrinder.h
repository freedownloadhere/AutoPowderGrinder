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

	std::shared_ptr<Player> player;

	Minecraft();
	bool isInitialized();

private:

	bool initialized{ false };
	JNIEnv* env{ nullptr };
	jclass mcClass{ nullptr };
	jobject mcClassInstance{ nullptr };

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
	std::string getInventoryItem(int slotIndex);

private:
	bool initialized{ false };
	Position position{ 0 };
	void* inventory{ nullptr };

	jclass EntityPlayerSPClass{ nullptr };
	jclass InventoryPlayerClass{ nullptr };
	jobject mcThePlayerInstance{ nullptr };
	jobject inventoryInstance{ nullptr };
	jobjectArray mainInventoryArray{ nullptr };
	jclass itemStackClass{ nullptr };
	JNIEnv* env{ nullptr };

	// Blocks around player

	bool initialize(
		JNIEnv* env,
		const jclass& mcClass,
		const jobject& mcClassInstance
	);

	void refreshPosition();
};
