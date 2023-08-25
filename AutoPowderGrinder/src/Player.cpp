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

	auto playerFieldID{ this->env->GetFieldID(mcClass, "h", "Lbew;") };
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

	auto inventoryID{ this->env->GetFieldID(EntityPlayerSPClass, "bi", "Lwm;") };
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
		printf("Could not get main inventory field ID\n");
		return false;
	}

	jobject mainInventoryObj = this->env->GetObjectField(this->inventoryInstance, mainInventoryFieldID);
	if (mainInventoryObj == nullptr)
	{
		printf("Could not get main inventory array object\n");
		return false;
	}

	this->mainInventoryArray = (jobjectArray)mainInventoryObj;

	this->itemStackClass = this->env->FindClass("zx");
	if (this->itemStackClass == nullptr)
	{
		std::cout << "Could not find class itemStack\n";
		return false;
	}

	this->refreshPosition();

	std::cout << "Initialized player obj\n";

	return true;
}

void AutoPowderGrinder::Minecraft::Player::refreshPosition()
{
	this->position.x = env->GetDoubleField(mcThePlayerInstance, env->GetFieldID(EntityPlayerSPClass, "s", "D"));
	this->position.y = env->GetDoubleField(mcThePlayerInstance, env->GetFieldID(EntityPlayerSPClass, "t", "D"));
	this->position.z = env->GetDoubleField(mcThePlayerInstance, env->GetFieldID(EntityPlayerSPClass, "u", "D"));
}

bool AutoPowderGrinder::Minecraft::Player::isInitialized()
{
	return this->initialized;
}

void AutoPowderGrinder::Minecraft::Player::sendChatMessage(const std::string& message)
{
	jclass chatCompClass{ nullptr };
	jobject chatComp{ nullptr };
	jmethodID addChatMessageID{ nullptr };
	jmethodID constructorID{ nullptr };
	jstring text{};

	text = env->NewStringUTF(message.c_str());

	chatCompClass = env->FindClass("fa"); // "fa" is ChatComponentText
	if (chatCompClass == nullptr)
	{
		std::cout << "Failed to get chat component class!\n";
		return;
	}

	constructorID = env->GetMethodID(chatCompClass, "<init>", "(Ljava/lang/String;)V");
	if (constructorID == nullptr)
	{
		std::cout << "Failed to get constructor for ChatComp class!\n";
		return;
	}

	chatComp = env->NewObject(chatCompClass, constructorID, text);
	if (chatComp == nullptr)
	{
		std::cout << "Failed to create chat component object!\n";
		return;
	}

	addChatMessageID = env->GetMethodID(EntityPlayerSPClass, "a", "(Leu;)V");
	if (addChatMessageID == nullptr)
	{
		std::cout << "Failed to get msg function ID!\n";
		return;
	}

	env->CallVoidMethod(mcThePlayerInstance, addChatMessageID, chatComp);

	env->DeleteLocalRef(text);
	env->DeleteLocalRef(chatComp);
}

std::string AutoPowderGrinder::Minecraft::Player::getInventoryItem(int slotIndex)
{
	jobject itemStackInSlot{ nullptr };
	jmethodID displayNameGetter{ nullptr };
	jstring itemName{ nullptr };

	itemStackInSlot = this->env->GetObjectArrayElement(this->mainInventoryArray, slotIndex);
	if (itemStackInSlot == nullptr)
	{
		std::cout << "Could not get item stack in slot " << slotIndex << "\n";
		return "Air";
	}

	displayNameGetter = this->env->GetMethodID(this->itemStackClass, "q", "()Ljava/lang/String;");
	if (displayNameGetter == nullptr)
	{
		std::cout << "Could not get the display name getter method\n";
		return "Air";
	}

	itemName = (jstring)this->env->CallObjectMethod(itemStackInSlot, displayNameGetter);

	std::string nativeName = this->env->GetStringUTFChars(itemName, 0);

	return nativeName;
}