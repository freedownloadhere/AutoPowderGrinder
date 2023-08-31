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

namespace apg
{
	constexpr double TO_RADIANS{ 57.29578049044297 }; // Pi over 180.
	constexpr int MAX_REACH{ 4 };

	class AutoPowderGrinder
	{
	public:
		AutoPowderGrinder();

		void run();

	private:
		class Minecraft;
		class Pathfinder;
		class BlockManager;
		class ItemManager;

		std::shared_ptr<Minecraft> minecraft{ nullptr };
		std::unique_ptr<BlockManager> blockManager{ nullptr };

		/*
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

	struct Vector3
	{
		double x{ 0 }, y{ 0 }, z{ 0 };

		static double distance(const Vector3& pos1, const Vector3& pos2);

		bool operator==(const Vector3& other) const;

		bool operator!=(const Vector3& other) const;

		Vector3 operator+(const Vector3& other) const;

		Vector3 operator-(const Vector3& other) const;

		Vector3 operator*(int multiplier) const;

		bool operator<(const Vector3& other) const; // For set. Might change to hash and unordered_set later.

		void truncate();

		/// <summary>
		/// Subtracts 1 from truncated negative values to
		/// solve an issue with the player position truncation.
		/// </summary>
		void truncate2();
	};
	constexpr Vector3 nullvector{ -566547550, -566547550, -566547550 };

	enum class EnumFacing
	{
		DOWN = 0,  // -y (0, -1, 0)
		UP = 1,	   // +y (0, 1, 0)
		NORTH = 2, // -z (0, 0, -1)
		SOUTH = 3, // +z (0, 0, 1)
		WEST = 4,  // -x (-1, 0, 0)
		EAST = 5   // +x (1, 0, 0)
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
		void updateYawPitch();
		std::string getItem(int index);
		std::string updateAndGetItem(int index);
		Vector3 getLookingAt();
		Vector3 getFootPosition();
		Vector3 getHeadPosition();
		EnumFacing getFacing();
		void setYawPitch(float yaw, float pitch);
		void leftClick();
		void rightClick();

	private:
		Vector3 position{ 0, 0, 0 };
		std::string inventory[36] = {};
		float yaw{ 0 }, pitch{ 0 };

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
			setRotation{ nullptr },
			getHorizontalFacing{ nullptr },
			getEnumFacingIndex{ nullptr },
			displayNameGetter{ nullptr };
		jfieldID
			objectMouseOver{ nullptr },
			positionX{ nullptr },
			positionY{ nullptr },
			positionZ{ nullptr },
			yawField{ nullptr },
			pitchField{ nullptr };

		inline static INPUT leftClickInput[2], rightClickInput[2];

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
		int getBlockID(const Vector3& pos);

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

	struct Block
	{
		int id;
		Vector3 pos;

		inline static std::set<int> 
			blocksToBreak = { 1, 14, 15, 16, 21, 56, 73, 74, 129 },
			blocksToOpen = { 54, 146 };

		bool toBreak() const;
		bool toOpen() const;
		bool isAir() const;
	};

	class AutoPowderGrinder::BlockManager
	{
	public:
		BlockManager(const std::shared_ptr<AutoPowderGrinder::Minecraft>& minecraft);

		bool isInitialized();
		void doRoutine();

	private:
		inline static const int
			MAX_QUEUE_SIZE = 12,
			MAX_SEARCH_DISTANCE_FRONT = 7,
			MAX_SEARCH_DISTANCE_SIDE = 3;

		const Vector3 d[6] =
		{
			{0, -1, 0},	// DOWN
			{0, 1, 0},  // UP
			{0, 0, -1},	// NORTH
			{0, 0, 1},	// SOUTH
			{-1, 0, 0},	// WEST
			{1, 0, 0}	// EAST
		};
		const Vector3 d_Straight[6][5] =
		{
			{},												// DOWN (do not access)
			{},												// UP (do not access)
			{d[4], d[4] + d[0], d[0], d[5] + d[0], d[4]},	// NORTH
			{d[5], d[5] + d[0], d[0], d[4] + d[0], d[4]},	// SOUTH
			{d[3], d[3] + d[0], d[0], d[2] + d[0], d[2]},	// WEST
			{d[2], d[2] + d[0], d[0], d[3] + d[0], d[3]}	// EAST
		};
		const Vector3 getBlockCenter = { 0.5, 0, 0.5 };

		std::shared_ptr<AutoPowderGrinder::Minecraft> minecraft{ nullptr };
		std::deque<Block> blockQueue;

		bool positionMeetsCriteria(
			const Vector3& pos,
			const EnumFacing& facing,
			const Vector3& playerPosition
		) const;
		bool checkBlockValidity(const Block& block) const;
		bool alreadyInQueue(const Block& block) const;

		Block toBlock(const Vector3& pos);

		//void findInitialStone();
		void queueBlocks();
		bool aimForBlock(const Block& targettedBlock);
		void actUponBlock(const Block& targettedBlock);

		bool initialized{ false };
		bool initialize(const std::shared_ptr<AutoPowderGrinder::Minecraft>& minecraft);
	};

	float clampAngle(float angle, float min, float max);
}
