#pragma once

#include <memory>
#include <jni.h>
#include <string>
#include <thread>
#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <math.h>
#include <queue>
#include <set>

class AutoPowderGrinder
{
public:
	AutoPowderGrinder();

	void run();

private:
	class Minecraft;
	class Pathfinder;
	class StoneMiner;
	class ChestOpener;
	class ItemManager;

	std::shared_ptr<Minecraft> minecraft{ nullptr };
	std::unique_ptr<StoneMiner> stoneMiner{ nullptr };

	/*
	std::unique_ptr<ChestOpener> chestOpener;
	std::unique_ptr<ItemManager> itemManager;
	std::unique_ptr<Pathfinder> pathfinder;*/

	bool initialized{ false };
	bool running{ false };

	bool initialize();
};

class AutoPowderGrinder::Minecraft
{
public:
	class Player;
	class World;

	std::shared_ptr<Player> player{ nullptr };
	std::shared_ptr<World> world{ nullptr };

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
	int x{ 0 }, y{ 0 }, z{ 0 };

	static double distance(const Position& pos1, const Position& pos2);

	bool operator==(const Position& other) const;

	bool operator!=(const Position& other) const;

	Position operator+(const Position& other) const;

	Position operator*(int multiplier) const;

	bool operator<(const Position& other) const;
};

class AutoPowderGrinder::Minecraft::Player
{
public:
	enum class EnumFacing
	{
		DOWN = 0,  // -y (0, -1, 0)
		UP = 1,	   // +y (0, 1, 0)
		NORTH = 2, // -z (0, 0, -1)
		SOUTH = 3, // +z (0, 0, 1)
		WEST = 4,  // -x (-1, 0, 0)
		EAST = 5   // +x (1, 0, 0)
	};

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
	Position getLookingAt();
	Position getPosition();
	EnumFacing getFacing();

private:
	Position position{ 0, 0, 0 };
	std::string inventory[36] = {};

	jclass 
		mcClass{ nullptr },
		EntityPlayerSPClass{ nullptr },
		InventoryPlayerClass{ nullptr },
		itemStackClass{ nullptr },
		enumFacingClass{ nullptr },
		chatCompClass{ nullptr };
	jobject 
		mcClassInstance{ nullptr },
		mcThePlayerInstance{ nullptr },
		inventoryInstance{ nullptr };
	jobjectArray 
		mainInventoryArray{ nullptr };
	jmethodID 
		getBlockPos{ nullptr },
		addChatMessage{ nullptr },
		messageConstructor{ nullptr },
		blockPosX{ nullptr },
		blockPosY{ nullptr },
		blockPosZ{ nullptr },
		getHorizontalFacing{ nullptr },
		getEnumFacingIndex{ nullptr },
		displayNameGetter{ nullptr };
	jfieldID
		objectMouseOver{ nullptr },
		positionX{ nullptr },
		positionY{ nullptr },
		positionZ{ nullptr };

	// TODO Blocks around player

	// Get enumfacing

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

class AutoPowderGrinder::StoneMiner
{
public:
	StoneMiner(const std::shared_ptr<AutoPowderGrinder::Minecraft>& minecraft);

	bool isInitialized();
	void doRoutine();

private:
	static const int
		STONE_ID = 1,
		MAX_QUEUE_SIZE = 9,
		MAX_SEARCH_DISTANCE_FRONT = 7,
		MAX_SEARCH_DISTANCE_SIDE = 3;

	const Position direction[6] =
	{
		{0, 1, 0},
		{0, -1, 0},
		{0, 0, -1},
		{0, 0, 1},
		{-1, 0, 0},
		{1, 0, 0}
	};

	std::shared_ptr<AutoPowderGrinder::Minecraft> minecraft{ nullptr };
	std::deque<Position> stoneToMine;

	bool isStone(const Position& pos);
	bool alreadyInQueue(const Position& pos);
	void findStone();
	bool positionMeetsCriteria(
		const Position& pos,
		AutoPowderGrinder::Minecraft::Player::EnumFacing facing,
		const Position& playerPosition
	);

	bool initialized{ false };
	bool initialize(const std::shared_ptr<AutoPowderGrinder::Minecraft>& minecraft);
};
