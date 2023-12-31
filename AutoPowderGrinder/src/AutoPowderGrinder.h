#pragma once

#include <memory>
#include <jni.h>
#include <jvmti.h>
#include <string>
#include <thread>
#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <math.h>
#include <queue>
#include <set>
#include <unordered_map>
#include <map>
#include <random>
#include <list>
#include <algorithm>
#include <chrono>

namespace apg
{
	constexpr double TO_RADIANS{ 57.29578049044297 }; // Pi over 180.
	constexpr int MAX_REACH{ 4 };

	class AutoPowderGrinder
	{
	public:
		class Minecraft;
		class Pathfinder;
		class BlockManager;
		class ItemManager;

		AutoPowderGrinder();

		void run();

	private:
		std::shared_ptr<Minecraft> minecraft{ nullptr };
		std::unique_ptr<BlockManager> blockManager{ nullptr };
		std::unique_ptr<Pathfinder> pathfinder{ nullptr };

		bool initialized{ false };
		bool running{ false };

		bool initialize();
	};

	jclass getClass(JNIEnv* env, const std::string& path);

	struct Vector3
	{
		Vector3();

		Vector3(double x, double y, double z);

		double x{ 0 }, y{ 0 }, z{ 0 };

		static double euclideanDistance(const Vector3& pos1, const Vector3& pos2);

		static double manhattanDistance(const Vector3& pos1, const Vector3& pos2);

		static Vector3 manhattanDistanceVector(const Vector3& pos1, const Vector3& pos2);

		bool operator==(const Vector3& other) const;

		bool operator!=(const Vector3& other) const;

		Vector3 operator+(const Vector3& other) const;

		Vector3 operator-(const Vector3& other) const;

		Vector3 operator*(int multiplier) const;

		bool operator<(const Vector3& other) const;

		friend std::ostream& operator<<(std::ostream& os, const Vector3& vector);

		std::string toString() const;

		void truncate();

		/// <summary>
		/// Subtracts 1 from truncated negative values to
		/// solve an issue with the player position truncation.
		/// </summary>
		void truncate2();

		static Vector3 truncate2(const Vector3& vector);
	};
	const Vector3 nullvector(-566547550, -566547550, -566547550);
	const Vector3 zerovector(0, 0, 0);
	const Vector3 enumFacingVec[6] =
	{
		{0, -1, 0},	// DOWN
		{0, 1, 0},  // UP
		{0, 0, -1},	// NORTH
		{0, 0, 1},	// SOUTH
		{-1, 0, 0},	// WEST
		{1, 0, 0}	// EAST
	};

	struct ViewAngles
	{
		double yaw{ 0 }, pitch{ 0 };

		ViewAngles operator+(const ViewAngles& other) const;

		ViewAngles operator-(const ViewAngles& other) const;

		ViewAngles operator/(int divisor) const;

		ViewAngles operator*(int multiplier) const;
	};
	const ViewAngles enumFacingToViewAngles[6] =
	{
		{0, -90},
		{0, 90},
		{180, 0},
		{0, 0},
		{90, 0},
		{-90, 0}
	};

	enum class EnumFacing
	{
		DOWN = 0,  // -y (0, -1, 0)
		UP = 1,	   // +y (0, 1, 0)
		NORTH = 2, // -z (0, 0, -1)
		SOUTH = 3, // +z (0, 0, 1)
		WEST = 4,  // -x (-1, 0, 0)
		EAST = 5   // +x (1, 0, 0)
	};

	struct Timer
	{
		std::chrono::steady_clock::time_point start, end;
		const char* topic;

		Timer();
		Timer(const char* topic);
		~Timer();
	};

	class AutoPowderGrinder::Minecraft
	{
	public:
		class Player;
		class World;
		class Chat;

		std::shared_ptr<Player> player{ nullptr };
		std::shared_ptr<World> world{ nullptr };
		std::shared_ptr<Chat> chat{ nullptr };

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

	class AutoPowderGrinder::Minecraft::Player
	{
	public:
		Player(
			JNIEnv* env,
			const jclass& mcClass,
			const jobject& mcClassInstance
		);

		bool isInitialized();
		
		void updateMainInventory();
		void updatePosition();
		void updateViewAngles();
		std::string getItem(int index);
		std::string updateAndGetItem(int index);
		Vector3 getLookingAt();
		Vector3 getBlockBelowPosition();
		Vector3 getFootPosition();
		Vector3 getHeadPosition();
		EnumFacing getFacing();
		void setViewAngles(const ViewAngles& newViewAngles);
		ViewAngles getViewAngles();
		void leftClick();
		void rightClick();
		jclass getEntityPlayerSPClass();
		jobject getMcThePlayerInstance();

	private:
		Vector3 position{ 0, 0, 0 };
		ViewAngles viewAngles{ 0, 0 };
		std::string inventory[36] = {};

		jclass
			mcClass{ nullptr },
			EntityPlayerSPClass{ nullptr },
			InventoryPlayerClass{ nullptr },
			itemStackClass{ nullptr },
			enumFacingClass{ nullptr };
		jobject
			mcClassInstance{ nullptr },
			mcThePlayerInstance{ nullptr },
			inventoryInstance{ nullptr };
		jobjectArray
			mainInventoryArray{ nullptr };
		jmethodID
			getBlockPos{ nullptr },
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

	class AutoPowderGrinder::Minecraft::Chat
	{
	public:
		Chat(
			JNIEnv* env,
			const jclass& mcClass,
			const jobject& mcClassInstance,
			const jclass& EntityPlayerSPClass,
			const jobject& mcThePlayerInstance
		);

		void sendMessageToPlayer(const std::string& message);
		void sendMessageFromPlayer(const std::string& message);
		std::string getLatestChatMessage();
		bool isInitialized();

	private:
		jclass
			guiInGameClass{ nullptr },
			guiNewChatClass{ nullptr },
			chatLineClass{ nullptr },
			listClass{ nullptr },
			chatCompClass{ nullptr };
		jobject
			mcThePlayerInstance{ nullptr },
			ingameGuiInstance{ nullptr },
			guiNewChatInstance{ nullptr },
			chatLinesInstance{ nullptr };
		jmethodID
			addChatMessage{ nullptr },
			sendChatMessage{ nullptr },
			messageConstructor{ nullptr },
			listSize{ nullptr },
			getUnformattedText{ nullptr },
			listGet{ nullptr },
			getChatComp{ nullptr };

		bool initialized{ false };
		JNIEnv* env{ nullptr };
		bool initialize(
			JNIEnv* env,
			const jclass& mcClass,
			const jobject& mcClassInstance,
			const jclass& EntityPlayerSPClass,
			const jobject& mcThePlayerInstance
		);
	};

	struct Block
	{
		int id;
		Vector3 pos;

		inline static std::set<int>
			blocksToBreak = { 1, 14, 15, 16, 21, 56, 73, 74, 129 },
			blocksToOpen = { 54, 146 },
			nonSolid = {
				0, 6, 8, 9, 10, 11, 27, 28, 
				30, 31, 32, 37, 38, 39, 40, 
				50, 51, 55, 59, 63, 65, 66, 
				68, 69, 70, 72, 75, 76, 77, 
				83, 90, 104, 105, 106, 115, 
				131, 132, 141, 142, 143, 147, 
				148, 157, 175, 176, 177, 207 
			};

		inline static std::shared_ptr<AutoPowderGrinder::Minecraft> minecraft{ nullptr };

		static Block toBlock(const Vector3& coordinates);

		bool toBreak() const;
		bool toOpen() const;
		bool isAir() const;
		bool isWalkable() const;
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

		const Vector3 directionalVector[6][6] =
		{
			{},															// DOWN (do not access)
			{},															// UP (do not access)
			{zerovector, enumFacingVec[4], enumFacingVec[4] + enumFacingVec[0], enumFacingVec[0], enumFacingVec[5] + enumFacingVec[0], enumFacingVec[4]},	// NORTH
			{zerovector, enumFacingVec[5], enumFacingVec[5] + enumFacingVec[0], enumFacingVec[0], enumFacingVec[4] + enumFacingVec[0], enumFacingVec[4]},	// SOUTH
			{zerovector, enumFacingVec[3], enumFacingVec[3] + enumFacingVec[0], enumFacingVec[0], enumFacingVec[2] + enumFacingVec[0], enumFacingVec[2]},	// WEST
			{zerovector, enumFacingVec[2], enumFacingVec[2] + enumFacingVec[0], enumFacingVec[0], enumFacingVec[3] + enumFacingVec[0], enumFacingVec[3]}	// EAST
		};
		const Vector3 getBlockCenter = { 0.5, 0, 0.5 };
		std::shared_ptr<AutoPowderGrinder::Minecraft> minecraft;

		std::deque<Block> blockQueue;

		bool positionIsValid(
			const Vector3& pos,
			const EnumFacing& facing,
			const Vector3& playerPosition
		) const;
		bool checkBlockValidity(const Block& block) const;
		bool alreadyInQueue(const Block& block) const;
		bool queueIsFull() const;

		void queueBlocks();
		void cleanUpQueue();
		bool aimForBlock(const Block& targettedBlock);
		void actUponBlock(const Block& targettedBlock);

		bool initialized{ false };
		bool initialize();
	};

	double clampAngle(double angle, double min, double max);

	struct AstarVector3 : public Vector3
	{
		double G{ 999999 }, H{ 0 }, F{ 0 };

		void setG(double value);
		void setH(double value);

		bool operator()(const AstarVector3& lhs, const AstarVector3& rhs) const;
	};

	class AutoPowderGrinder::Pathfinder
	{
	public:
		Pathfinder(const std::shared_ptr<Minecraft>& minecraft);

		void moveTo(const Vector3& target);
		std::list<Vector3> makePath(const Vector3& start, const Vector3& target);
		void traversePath(const std::list<Vector3>& path);

		bool isInitialized();
	private:
		Vector3 directionalVector[12] =
		{
			{0, 0, -1}, // NORTH
			{1, 0, 0},  // EAST
			{0, 0, 1},  // SOUTH
			{-1, 0, 0}, // WEST
			{0, -1, -1}, // NORTH + DOWN
			{1, -1, 0},  // EAST + DOWN
			{0, -1, 1},  // SOUTH + DOWN
			{-1, -1, 0}, // WEST + DOWN
			{0, 1, -1}, // NORTH + UP
			{1, 1, 0},  // EAST + UP
			{0, 1, 1},  // SOUTH + UP
			{-1, 1, 0}, // WEST + UP
		};
		const Vector3 upOne{ 0, 1, 0 };
		const Vector3 upTwo{ 0, 2, 0 };
		const Vector3 getBlockGoalpoint{ 0.5, 1, 0.5 };
		const double errorMargin{ 0.3 };

		/// <summary>
		/// Lines are, in order:
		/// W, A, S and D.
		/// Col 0 is press, col 1 is release.
		/// </summary>
		INPUT wasd[4][2];

		std::shared_ptr<Minecraft> minecraft{ nullptr };

		std::mt19937 shuffler;
		std::map<Vector3, bool> walkableBlockCache;

		std::vector<std::pair<Vector3, int>> makeWalkMap(const std::list<Vector3>& path);
		bool listContains(const AstarVector3& element, const std::vector<AstarVector3>& heap);
		bool isWalkable(const AstarVector3& coordinates);

		bool initialized{ false };
		bool initialize(const std::shared_ptr<Minecraft>& minecraft);
	};
}
