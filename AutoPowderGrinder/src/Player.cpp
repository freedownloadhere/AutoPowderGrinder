#include "AutoPowderGrinder.h"

using namespace apg;

AutoPowderGrinder::Minecraft::Player::Player(
	JNIEnv* env,
	const jclass& mcClass,
	const jobject& mcClassInstance
)
{
	this->initialized = this->initialize(
		env, mcClass, mcClassInstance
	);

	if (!this->initialized)
		std::cout << "An error occured while initializing Player\n";
}

bool AutoPowderGrinder::Minecraft::Player::initialize(
	JNIEnv* env,
	const jclass& mcClass,
	const jobject& mcClassInstance
)
{
	this->env = env;
	this->mcClass = mcClass;
	this->mcClassInstance = mcClassInstance;

	this->EntityPlayerSPClass = this->env->FindClass("bew");
	if (this->EntityPlayerSPClass == nullptr)
	{
		std::cout << "Failed to get class EntityPlayerSP\n";
		return false;
	}

	jfieldID playerFieldID{ this->env->GetFieldID(mcClass, "h", "Lbew;") };
	if (playerFieldID == nullptr)
	{
		std::cout << "Failed to get object field ID thePlayer\n";
		return false;
	}

	this->mcThePlayerInstance = this->env->GetObjectField(mcClassInstance, playerFieldID);
	if (this->mcThePlayerInstance == nullptr)
	{
		std::cout << "Failed to get object field thePlayer\n";
		return false;
	}

	this->InventoryPlayerClass = this->env->FindClass("wm");
	if (this->InventoryPlayerClass == nullptr)
	{
		std::cout << "Failed to get class InventoryPlayer\n";
		return false;
	}

	jfieldID inventoryID{ this->env->GetFieldID(EntityPlayerSPClass, "bi", "Lwm;") };
	if (inventoryID == nullptr)
	{
		std::cout << "Failed to get inventory field ID!\n";
		return false;
	}

	this->inventoryInstance = this->env->GetObjectField(mcThePlayerInstance, inventoryID);
	if (inventoryInstance == nullptr)
	{
		std::cout << "Failed to get object field inventory\n";
		return false;
	}

	jfieldID mainInventoryFieldID = this->env->GetFieldID(this->InventoryPlayerClass, "a", "[Lzx;");
	if (mainInventoryFieldID == nullptr)
	{
		std::cout << "Could not get main inventory field ID\n";
		return false;
	}

	jobject mainInventoryObj = this->env->GetObjectField(this->inventoryInstance, mainInventoryFieldID);
	if (mainInventoryObj == nullptr)
	{
		std::cout << "Could not get main inventory array object\n";
		return false;
	}

	this->mainInventoryArray = static_cast<jobjectArray>(mainInventoryObj);

	this->itemStackClass = this->env->FindClass("zx");
	if (this->itemStackClass == nullptr)
	{
		std::cout << "Could not find class itemStack\n";
		return false;
	}

	this->displayNameGetter = this->env->GetMethodID(this->itemStackClass, "q", "()Ljava/lang/String;");
	if (this->displayNameGetter == nullptr)
	{
		std::cout << "Could not get the display name getter method\n";
		return false;
	}

	this->positionX = this->env->GetFieldID(EntityPlayerSPClass, "s", "D");
	if (this->positionX == nullptr)
	{
		std::cout << "Could not get player X position field ID\n";
		return false;
	}

	this->positionY = this->env->GetFieldID(EntityPlayerSPClass, "t", "D");
	if (this->positionY == nullptr)
	{
		std::cout << "Could not get player Y position field ID\n";
		return false;
	}

	this->positionZ = this->env->GetFieldID(EntityPlayerSPClass, "u", "D");
	if (this->positionZ == nullptr)
	{
		std::cout << "Could not get player Z position field ID\n";
		return false;
	}

	this->yawField = this->env->GetFieldID(EntityPlayerSPClass, "bN", "F");
	if (this->yawField == nullptr)
	{
		std::cout << "Could not get player yaw field\n";
		return false;
	}

	this->pitchField = this->env->GetFieldID(EntityPlayerSPClass, "bO", "F");
	if (this->pitchField == nullptr)
	{
		std::cout << "Could not get player yaw field\n";
		return false;
	}

	jclass objMouseOverClass = this->env->FindClass("auh");
	if (objMouseOverClass == nullptr)
	{
		std::cout << "Could not get the objectMouseOver class\n";
		return false;
	}

	this->objectMouseOver = this->env->GetFieldID(this->mcClass, "s", "Lauh;");
	if (objectMouseOver == nullptr)
	{
		std::cout << "Could not get the objectMouseOver minecraft class field\n";
		return false;
	}

	this->getBlockPos = this->env->GetMethodID(objMouseOverClass, "a", "()Lcj;");
	if (this->getBlockPos == nullptr)
	{
		std::cout << "Could not get the getBlockPos method\n";
		return false;
	}

	jclass blockPosClass = this->env->FindClass("cj");
	if (blockPosClass == nullptr)
	{
		std::cout << "Could not get block pos class\n";
		return false;
	}

	this->blockPosX = this->env->GetMethodID(blockPosClass, "n", "()I");
	if (this->blockPosX == nullptr)
	{
		std::cout << "Could not get the getX block pos method\n";
		return false;
	}

	this->blockPosY = this->env->GetMethodID(blockPosClass, "o", "()I");
	if (this->blockPosY == nullptr)
	{
		std::cout << "Could not get the getY block pos method\n";
		return false;
	}

	this->blockPosZ = this->env->GetMethodID(blockPosClass, "p", "()I");
	if (this->blockPosZ == nullptr)
	{
		std::cout << "Could not get the getZ block pos method\n";
		return false;
	}

	this->setRotation = this->env->GetMethodID(this->env->FindClass("pk"), "b", "(FF)V");
	if (this->setRotation == nullptr)
	{
		std::cout << "Could not get the setRotation method\n";
		return false;
	}

	this->enumFacingClass = this->env->FindClass("cq");
	if (this->enumFacingClass == nullptr)
	{
		std::cout << "Could not get the enumFacing class\n";
		return false;
	}

	this->getHorizontalFacing = this->env->GetMethodID(this->EntityPlayerSPClass, "aP", "()Lcq;");
	if (this->getHorizontalFacing == nullptr)
	{
		std::cout << "Could not get the getHorizontalFacing method\n";
		return false;
	}

	this->getEnumFacingIndex = this->env->GetMethodID(this->enumFacingClass, "a", "()I");
	if (this->getEnumFacingIndex == nullptr)
	{
		std::cout << "Could not get the enumfacing getIndex method\n";
		return false;
	}

	AutoPowderGrinder::Minecraft::Player::leftClickInput[0].type = INPUT_MOUSE;
	AutoPowderGrinder::Minecraft::Player::leftClickInput[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	AutoPowderGrinder::Minecraft::Player::leftClickInput[1].type = INPUT_MOUSE;
	AutoPowderGrinder::Minecraft::Player::leftClickInput[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	AutoPowderGrinder::Minecraft::Player::rightClickInput[0].type = INPUT_MOUSE;
	AutoPowderGrinder::Minecraft::Player::rightClickInput[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	AutoPowderGrinder::Minecraft::Player::rightClickInput[1].type = INPUT_MOUSE;
	AutoPowderGrinder::Minecraft::Player::rightClickInput[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

	this->updatePosition();
	this->updateMainInventory();

	return true;
}

void AutoPowderGrinder::Minecraft::Player::updatePosition()
{
	this->position.x = this->env->GetDoubleField(this->mcThePlayerInstance, this->positionX);
	this->position.y = this->env->GetDoubleField(this->mcThePlayerInstance, this->positionY);
	this->position.z = this->env->GetDoubleField(this->mcThePlayerInstance, this->positionZ);
}

void AutoPowderGrinder::Minecraft::Player::updateViewAngles()
{
	this->viewAngles.yaw = this->env->GetFloatField(this->mcThePlayerInstance, this->yawField);
	this->viewAngles.pitch = this->env->GetFloatField(this->mcThePlayerInstance, this->pitchField);

	this->viewAngles.yaw = apg::clampAngle(this->viewAngles.yaw, -180, 180);
}

void AutoPowderGrinder::Minecraft::Player::setViewAngles(const ViewAngles& newViewAngles)
{
	this->env->CallVoidMethod(this->mcThePlayerInstance, this->setRotation, newViewAngles.yaw, newViewAngles.pitch);
}

ViewAngles AutoPowderGrinder::Minecraft::Player::getViewAngles()
{
	this->updateViewAngles();

	return this->viewAngles;
}

bool AutoPowderGrinder::Minecraft::Player::isInitialized()
{
	return this->initialized;
}

void AutoPowderGrinder::Minecraft::Player::updateMainInventory()
{
	jobject itemStackInSlot{ nullptr };
	jstring itemName{ nullptr };

	for (int index = 0; index < 36; ++index)
	{
		itemStackInSlot = this->env->GetObjectArrayElement(this->mainInventoryArray, index);
		if (itemStackInSlot == nullptr)
		{
			this->inventory[index] = "Air";
			continue;
		}

		itemName = static_cast<jstring>(this->env->CallObjectMethod(itemStackInSlot, displayNameGetter));

		this->inventory[index] = this->env->GetStringUTFChars(itemName, 0);
	}
}

std::string AutoPowderGrinder::Minecraft::Player::getItem(int index)
{
	if (!this->initialized)
	{
		std::cout << "The player object was not initialized properly\n";
		return "";
	}

	if (index >= 0 and index < 36)
		return this->inventory[index];
	else
		return "";
}

std::string AutoPowderGrinder::Minecraft::Player::updateAndGetItem(int index)
{
	if (!this->initialized)
	{
		std::cout << "The player object was not initialized properly\n";
		return "";
	}

	this->updateMainInventory();

	if (index >= 0 and index < 36)
		return this->inventory[index];
	else
		return "";
}

Vector3 AutoPowderGrinder::Minecraft::Player::getLookingAt()
{
	jobject mouseOverInstance{ nullptr };
	jobject blockPos{ nullptr };
	Vector3 pos{ 0, 0, 0 };

	mouseOverInstance = this->env->GetObjectField(this->mcClassInstance, this->objectMouseOver);
	if (mouseOverInstance == nullptr)
	{
		std::cout << "Could not get the objMouseOver instance\n";
		return pos;
	}

	blockPos = this->env->CallObjectMethod(mouseOverInstance, this->getBlockPos);
	if (blockPos == nullptr)
	{
		std::cout << "Could not get the block position the player is looking at\n";
		return pos;
	}

	pos.x = this->env->CallIntMethod(blockPos, this->blockPosX);
	pos.y = this->env->CallIntMethod(blockPos, this->blockPosY);
	pos.z = this->env->CallIntMethod(blockPos, this->blockPosZ);

	return pos;
}

EnumFacing AutoPowderGrinder::Minecraft::Player::getFacing()
{
	jobject facing{ nullptr };
	int index = 0;

	facing = this->env->CallObjectMethod(this->mcThePlayerInstance, this->getHorizontalFacing);
	if (facing == nullptr)
	{
		std::cout << "Could not get the direction the player is facing\n";
		return EnumFacing::DOWN;
	}

	index = this->env->CallIntMethod(facing, this->getEnumFacingIndex);
	if (index == 0)
		std::cout << "Invalid horizontal enumfacing index\n";

	return (EnumFacing)index;
}

Vector3 AutoPowderGrinder::Minecraft::Player::getBlockBelowPosition()
{
	this->updatePosition();

	Vector3 result( this->position.x, this->position.y - 1, this->position.z );
	result.truncate2();

	return result;
}

Vector3 AutoPowderGrinder::Minecraft::Player::getFootPosition()
{
	this->updatePosition();

	return this->position;
}

Vector3 AutoPowderGrinder::Minecraft::Player::getHeadPosition()
{
	this->updatePosition();

	Vector3 result( this->position.x, this->position.y + 1, this->position.z );
	return result;
}

void AutoPowderGrinder::Minecraft::Player::leftClick()
{
	SendInput(1, &AutoPowderGrinder::Minecraft::Player::leftClickInput[0], sizeof(INPUT));
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	SendInput(1, &AutoPowderGrinder::Minecraft::Player::leftClickInput[1], sizeof(INPUT));
}

void AutoPowderGrinder::Minecraft::Player::rightClick()
{
	SendInput(1, &AutoPowderGrinder::Minecraft::Player::rightClickInput[0], sizeof(INPUT));
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	SendInput(1, &AutoPowderGrinder::Minecraft::Player::rightClickInput[1], sizeof(INPUT));
}

jclass AutoPowderGrinder::Minecraft::Player::getEntityPlayerSPClass()
{
	return this->EntityPlayerSPClass;
}

jobject AutoPowderGrinder::Minecraft::Player::getMcThePlayerInstance()
{
	return this->mcThePlayerInstance;
}