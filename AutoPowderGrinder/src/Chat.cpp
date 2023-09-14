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

	auto chatLinesField = this->env->GetFieldID(this->guiNewChatClass, "h", "Ljava/util/List;");
	if (chatLinesField == nullptr)
	{
		std::cout << "Could not get the chatLines field\n";
		return false;
	}

	this->chatLinesInstance = this->env->GetObjectField(this->guiNewChatInstance, chatLinesField);
	if (this->chatLinesInstance == nullptr)
	{
		std::cout << "Could not get the chatLines object\n";
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

	this->chatLineClass = this->env->FindClass("ava");
	if (this->chatLineClass == nullptr)
	{
		std::cout << "Could not get the chatLine class\n";
		return false;
	}

	this->getChatComp = this->env->GetMethodID(this->chatLineClass, "a", "()Leu;");
	if (this->getChatComp == nullptr)
	{
		std::cout << "Could not get the chat component field\n";
		return false;
	}

	this->getFormattedText = this->env->GetMethodID(this->chatCompClass, "d", "()Ljava/lang/String;");
	if (this->getFormattedText == nullptr)
	{
		std::cout << "Could not get the unformatted text method\n";
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

/// <summary>
/// A mess to fix later (doesn't work)
/// </summary>
std::string AutoPowderGrinder::Minecraft::Chat::getLatestChatMessage()
{
	int size = this->env->CallIntMethod(this->chatLinesInstance, this->listSize);
	if (size == 0)
		return "No chat message to display";

	jobject lastChatLine = this->env->CallObjectMethod(this->chatLinesInstance, this->listGet, 0);
	if (lastChatLine == nullptr)
	{
		return "lastChatLine is a nullptr";
	}

	jobject chatComponent = this->env->CallObjectMethod(lastChatLine, this->getChatComp);
	if (chatComponent == nullptr)
	{
		return "chatComponent is a nullptr";
	}

	jstring formattedText = (jstring)this->env->CallObjectMethod(chatComponent, this->getFormattedText);
	if (formattedText == nullptr)
	{
		return "formattedText is a nullptr";
	}

	auto result = this->env->GetStringChars(formattedText, 0);
	if (result == nullptr)
	{
		return "result is a nullptr";
	}
	int resultSize = this->env->GetStringLength(formattedText);

	for (int i = 0; i < resultSize; ++i)
	{
		std::wcout << (wchar_t)result[i];
	}
	std::wcout << '\n';

	this->env->ReleaseStringChars(formattedText, result);

	//return result;
	return "debug string";
}