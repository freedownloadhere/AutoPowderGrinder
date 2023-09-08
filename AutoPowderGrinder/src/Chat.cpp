#include "AutoPowderGrinder.h"

using namespace apg;

AutoPowderGrinder::Minecraft::Chat::Chat(
	JNIEnv* env,
	const jclass& mcClass,
	const jobject& mcClassInstance,
	const jclass& EntityPlayerSPClass,
	const jobject& mcThePlayerInstance
)
{
	this->initialized = this->initialize(env, mcClass, mcClassInstance, EntityPlayerSPClass, mcThePlayerInstance);

	if (!this->initialized)
		std::cout << "An error occured while initializing Chat\n";
}

bool AutoPowderGrinder::Minecraft::Chat::initialize(
	JNIEnv* env,
	const jclass& mcClass,
	const jobject& mcClassInstance,
	const jclass& EntityPlayerSPClass,
	const jobject& mcThePlayerInstance
)
{
	this->env = env;
	this->mcThePlayerInstance = mcThePlayerInstance;

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
	this->sendChatMessage = this->env->GetMethodID(EntityPlayerSPClass, "e", "(Ljava/lang/String;)V");
	if (this->sendChatMessage == nullptr)
	{
		std::cout << "Could not get the sendChatMessage method\n";
		return false;
	}

	this->guiInGameClass = this->env->FindClass("avo");
	if (this->guiInGameClass == nullptr)
	{
		std::cout << "Could not get the GuiInGame class\n";
		return false;
	}

	this->guiNewChatClass = this->env->FindClass("avt");
	if (this->guiNewChatClass == nullptr)
	{
		std::cout << "Could not get the GuiNewChat class\n";
		return false;
	}

	this->listClass = this->env->FindClass("java/util/List");
	if (this->listClass == nullptr)
	{
		std::cout << "Could not get the List class\n";
		return false;
	}

	auto ingameGuiFieldID = this->env->GetFieldID(mcClass, "q", "Lavo;");
	if (ingameGuiFieldID == nullptr)
	{
		std::cout << "Could not get the ingameGui field\n";
		return false;
	}

	this->ingameGuiInstance = this->env->GetObjectField(mcClassInstance, ingameGuiFieldID);
	if (this->ingameGuiInstance == nullptr)
	{
		std::cout << "Could not get the ingameGui object\n";
		return false;
	}

	auto guiNewChatFieldID = this->env->GetFieldID(this->guiInGameClass, "l", "Lavt;");
	if (guiNewChatFieldID == nullptr)
	{
		std::cout << "Could not get the GuiNewChat field\n";
		return false;
	}

	this->guiNewChatInstance = this->env->GetObjectField(this->ingameGuiInstance, guiNewChatFieldID);
	if (this->guiNewChatInstance == nullptr)
	{
		std::cout << "Could not get the GuiNewChat object\n";
		return false;
	}

	auto sentMessagesFieldID = this->env->GetFieldID(this->guiNewChatClass, "g", "Ljava/util/List;");
	if (sentMessagesFieldID == nullptr)
	{
		std::cout << "Could not get the sentMessages field\n";
		return false;
	}

	this->sentMessagesInstance = this->env->GetObjectField(this->guiNewChatInstance, sentMessagesFieldID);
	if (this->sentMessagesInstance == nullptr)
	{
		std::cout << "Could not get the sentMessages object\n";
		return false;
	}

	this->listGet = this->env->GetMethodID(this->listClass, "get", "(I)Ljava/lang/Object;");
	if (this->listGet == nullptr)
	{
		std::cout << "Could not get the listGet method\n";
		return false;
	}

	this->listSize = this->env->GetMethodID(this->listClass, "size", "()I");
	if (this->listSize == nullptr)
	{
		std::cout << "Could not get the listSize method\n";
		return false;
	}

	return true;
}

bool AutoPowderGrinder::Minecraft::Chat::isInitialized()
{
	return this->initialized;
}

void AutoPowderGrinder::Minecraft::Chat::sendMessageToPlayer(const std::string& message)
{
	jobject chatComp{ nullptr };
	jstring text{ nullptr };

	text = this->env->NewStringUTF(message.c_str());

	chatComp = this->env->NewObject(chatCompClass, this->messageConstructor, text);
	if (chatComp == nullptr)
	{
		std::cout << "Failed to create chat component object\n";
		return;
	}

	this->env->CallVoidMethod(this->mcThePlayerInstance, this->addChatMessage, chatComp);

	this->env->DeleteLocalRef(text);
	this->env->DeleteLocalRef(chatComp);
}

void AutoPowderGrinder::Minecraft::Chat::sendMessageFromPlayer(const std::string& message)
{
	jstring text{ nullptr };

	text = this->env->NewStringUTF(message.c_str());

	this->env->CallVoidMethod(this->mcThePlayerInstance, this->sendChatMessage, text);

	this->env->DeleteLocalRef(text);
}

std::string AutoPowderGrinder::Minecraft::Chat::getLatestChatMessage()
{
	int size = this->env->CallIntMethod(this->sentMessagesInstance, this->listSize);

	if (size == 0)
		return "No chat message to display";

	jstring lastMessage = (jstring)this->env->CallObjectMethod(this->sentMessagesInstance, this->listGet, size - 1);

	return this->env->GetStringUTFChars(lastMessage, 0);
}