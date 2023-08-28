#include "AutoPowderGrinder.h"

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

	this->mainInventoryArray = (jobjectArray)mainInventoryObj;

	this->itemStackClass = this->env->FindClass("zx");
	if (this->itemStackClass == nullptr)
	{
		std::cout << "Could not find class itemStack\n";
		return false;
	}

	this->chatCompClass = env->FindClass("fa");
	if (this->chatCompClass == nullptr)
	{
		std::cout << "Failed to get chat component class!\n";
		return false;
	}

	this->addChatMessage = env->GetMethodID(EntityPlayerSPClass, "a", "(Leu;)V");
	if (this->addChatMessage == nullptr)
	{
		std::cout << "Failed to get msg function ID!\n";
		return false;
	}

	this->messageConstructor = env->GetMethodID(chatCompClass, "<init>", "(Ljava/lang/String;)V");
	if (this->messageConstructor == nullptr)
	{
		std::cout << "Failed to get constructor for ChatComp class!\n";
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

bool AutoPowderGrinder::Minecraft::Player::isInitialized()
{
	return this->initialized;
}

void AutoPowderGrinder::Minecraft::Player::sendChatMessage(const std::string& message)
{
	if (!this->initialized)
	{
		std::cout << "The player object was not initialized properly\n";
		return;
	}

	jobject chatComp{ nullptr };
	jstring text{ nullptr };

	text = env->NewStringUTF(message.c_str());

	chatComp = env->NewObject(chatCompClass, this->messageConstructor, text);
	if (chatComp == nullptr)
	{
		std::cout << "Failed to create chat component object\n";
		return;
	}

	env->CallVoidMethod(this->mcThePlayerInstance, this->addChatMessage, chatComp);

	env->DeleteLocalRef(text);
	env->DeleteLocalRef(chatComp);
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