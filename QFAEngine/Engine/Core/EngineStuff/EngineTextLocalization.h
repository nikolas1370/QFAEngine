#pragma once

#include <Tools/Stuff.h>
#include <UI/Text.h>

/*   move inside */
struct STextOther
{
	const char* languageId;
	std::u32string text;
};
struct SText
{
	const char* TextId = nullptr;
	std::u32string Text;
};
/*--- end ---*/

class QFALocalization;
class QFATextLocalization;
struct QFAEngineLanguage
{
	friend QFALocalization;
	friend QFATextLocalization;
	inline static const char* DefaultName = "eng";
	inline static const std::u32string DefaultDisplayName = U"English";

private:
	const char* Name;// id
	std::u32string DisplayName;
	QFAEngineLanguage(const char* languageId, const std::u32string& displayName)
	{
		Name = languageId;
		DisplayName = displayName;
	}
	
};

class QFAEngineLocalization;
class QFAEngineTextLocalization
{
	friend QFAText;
	friend QFALocalization;
protected:
	SText Text;
	// all QFAText add in here after QFAText->SetText(QFAEngineTextLocalization*)
	std::vector<QFAText*> QFATextList;
	// store variant this text for other Language
	std::vector<STextOther> TextListOtherLanguage;
	void AddMe(QFAText* text)
	{
		QFATextList.push_back(text);
	}
	void RemoveMe(QFAText* text)
	{
		for (size_t i = 0; i < QFATextList.size(); i++)
		{
			if (QFATextList[i] == text)
			{
				QFATextList.erase(QFATextList.begin() + i);
				return;
			}
		}
	}

	void Notify()
	{
		for (size_t i = 0; i < QFATextList.size(); i++)
			QFATextList[i]->TextLocalizationChange();
	}

public:
	virtual const std::u32string& GetCurentString() = 0;
};

class QFAEXPORT QFAEngineLocalization
{
	static QFAEngineLocalization* GameModuleLocal;
protected:
	std::vector<QFAEngineTextLocalization*> TextList;
	std::vector<QFAEngineLanguage*> LanguageList;

	QFAEngineLanguage* Curentlanguage = nullptr;
	static void SetLocalizationInstance(QFAEngineLocalization* localInstance);

	QFAEngineLocalization();
	virtual ~QFAEngineLocalization();
};