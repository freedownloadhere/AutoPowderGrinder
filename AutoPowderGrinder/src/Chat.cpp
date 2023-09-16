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
		std::cout << "[-] An error occured while initializing Chat\n";
	else
		std::cout << "[+] Successfully initialized Chat\n";
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

	this->chatCompClass = apg::getClass(this->env, "net/minecraft/util/ChatComponentText");
	if (this->chatCompClass == nullptr)
	{
		std::cout << "	[-] Failed to get chat component class\n";
		return false;
	}

	this->guiInGameClass = apg::getClass(this->env, "net/minecraft/client/gui/GuiIngame");
	if (this->guiInGameClass == nullptr)
	{
		std::cout << "	[-] Could not get the GuiIngame class\n";
		return false;
	}

	this->guiNewChatClass = apg::getClass(this->env, "net/minecraft/client/gui/GuiNewChat");
	if (this->guiNewChatClass == nullptr)
	{
		std::cout << "	[-] Could not get the GuiNewChat class\n";
		return false;
	}

	this->chatLineClass = apg::getClass(this->env, "net/minecraft/client/gui/ChatLine");
	if (this->chatLineClass == nullptr)
	{
		std::cout << "	[-] Could not get the chatLine class\n";
		return false;
	}

	this->listClass = apg::getClass(this->env, "java/util/List");
	if (this->listClass == nullptr)
	{
		std::cout << "	[-] Could not get the List class\n";
		return false;
	}

	this->addChatMessage = this->env->GetMethodID(EntityPlayerSPClass, "func_145747_a", "(Lnet/minecraft/util/IChatComponent;)V");
	if (this->addChatMessage == nullptr)
	{
		std::cout << "	[-] Failed to get addChatMessage method\n";
		return false;
	}

	this->messageConstructor = this->env->GetMethodID(this->chatCompClass, "<init>", "(Ljava/lang/String;)V");
	if (this->messageConstructor == nullptr)
	{
		std::cout << "	[-] Failed to get constructor for IChatComponent class\n";
		return false;
	}

	this->sendChatMessage = this->env->GetMethodID(EntityPlayerSPClass, "func_71165_d", "(Ljava/lang/String;)V");
	if (this->sendChatMessage == nullptr)
	{
		std::cout << "	[-] Could not get the sendChatMessage method\n";
		return false;
	}

	auto ingameGuiFieldID = this->env->GetFieldID(mcClass, "field_71456_v", "Lnet/minecraft/client/gui/GuiIngame;");
	if (ingameGuiFieldID == nullptr)
	{
		std::cout << "	[-] Could not get the ingameGui field\n";
		return false;
	}

	this->ingameGuiInstance = this->env->GetObjectField(mcClassInstance, ingameGuiFieldID);
	if (this->ingameGuiInstance == nullptr)
	{
		std::cout << "	[-] Could not get the ingameGui object\n";
		return false;
	}

	auto persistentChatFieldID = this->env->GetFieldID(this->guiInGameClass, "field_73840_e", "Lnet/minecraft/client/gui/GuiNewChat;");
	if (persistentChatFieldID == nullptr)
	{
		std::cout << "	[-] Could not get the GuiNewChat field\n";
		return false;
	}

	this->guiNewChatInstance = this->env->GetObjectField(this->ingameGuiInstance, persistentChatFieldID);
	if (this->guiNewChatInstance == nullptr)
	{
		std::cout << "	[-] Could not get the GuiNewChat object\n";
		return false;
	}

	auto chatLinesField = this->env->GetFieldID(this->guiNewChatClass, "field_146252_h", "Ljava/util/List;");
	if (chatLinesField == nullptr)
	{
		std::cout << "	[-] Could not get the chatLines field\n";
		return false;
	}

	this->chatLinesInstance = this->env->GetObjectField(this->guiNewChatInstance, chatLinesField);
	if (this->chatLinesInstance == nullptr)
	{
		std::cout << "	[-] Could not get the chatLines object\n";
		return false;
	}

	this->listGet = this->env->GetMethodID(this->listClass, "get", "(I)Ljava/lang/Object;");
	if (this->listGet == nullptr)
	{
		std::cout << "	[-] Could not get the listGet method\n";
		return false;
	}

	this->listSize = this->env->GetMethodID(this->listClass, "size", "()I");
	if (this->listSize == nullptr)
	{
		std::cout << "	[-] Could not get the listSize method\n";
		return false;
	}

	this->getChatComp = this->env->GetMethodID(this->chatLineClass, "func_151461_a", "()Lnet/minecraft/util/IChatComponent;");
	if (this->getChatComp == nullptr)
	{
		std::cout << "	[-] Could not get the getChatComponent method\n";
		return false;
	}

	this->getUnformattedText = this->env->GetMethodID(this->chatCompClass, "func_150260_c", "()Ljava/lang/String;");
	if (this->getUnformattedText == nullptr)
	{
		std::cout << "	[-] Could not get the unformatted text method\n";
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

	jstring formattedText = (jstring)this->env->CallObjectMethod(chatComponent, this->getUnformattedText);
	if (formattedText == nullptr)
	{
		return "formattedText is a nullptr";
	}

	auto result = this->env->GetStringUTFChars(formattedText, 0);

	return result;
}