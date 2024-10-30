#pragma once
// this file only for game module
#include <EngineStuff/EngineTextLocalization.h>
#include <string>
#include <vector>

/*
    add new text in localization system ( in defauld language )

    set it in global or inside class/struct
    don't set in function

    can use it like

    class Some
    {
        LOCALTEXT(ID_name, "some default text")

        void fun()
        {
            QFAText text;
            text.SetText(ID_name);

            LOCALTEXT(ID_name, "some default text") // ERROR
        }
    }

    language by defauld in LOCALTEXT is "eng"
    don't dublicate TextId( you can, but don't do it )

    if wanna add text without using macros inside function next example for you,
    {
        QFATextLocalization* TextId = new QFATextLocalization("TextId", "DefaultText");
        // after use in QFAText
    }
*/
#define LOCALTEXT(TextId, DefaultText) inline static QFATextLocalization TextId{#TextId, U##DefaultText};

/*
    example 

    LOCALTEXT(ID_name, "some default text")
    class Some
    {
        LOCALTEXT(ID_name_2, "some default text")

        void fun()
        {
            QFALocalization* local = QFALocalization::GetLocalizationInstance();
            local->AddLanguage("ua", U"Ukrainian"); 
            Hedgehog.SetTextFor("ua", U"ua text");


            local->ChangeLanguage("ua"); 
            // after it ID_name text be display like U"ua text"
            // ID_name_2 be display U"some default text" because don't add "ua" text for ID_name_2 text
            local->ChangeLanguage("eng");
            //  ID_name store U"some default text"
        }
    }
*/

class QFAText;
class QFAClass;
class QFATextLocalization;
class QFALocalization : public QFAEngineLocalization
{
    friend QFATextLocalization;
    // need for engine
    inline static QFALocalization* Local = nullptr;
    void AddTextInside(QFAEngineTextLocalization* text)
    {
        TextList.push_back(text);
        if (LanguageList.size() == 0)
            AddLanguage(QFAEngineLanguage::DefaultName, QFAEngineLanguage::DefaultDisplayName);
    }

public:

    static QFALocalization* GetLocalizationInstance()
    {
        if (Local)
            return Local;
        else
            return Local = new QFALocalization;
    }

    // add new Language if QFALanguage(languageId) not exist
    QFAEngineLanguage* AddLanguage(const char* languageId, const std::u32string& languageName)
    {
        QFALocalization* local = QFALocalization::GetLocalizationInstance();
        if (QFAEngineLanguage* lan = local->FindLanguage(languageId))
            return nullptr;

        QFAEngineLanguage* lan = new QFAEngineLanguage{ languageId, languageName };
        local->LanguageList.push_back(lan);
        if (local->LanguageList.size() == 1)
            local->Curentlanguage = lan;
                
        return lan;
    }

    /*
        activate languageId for ui
        if QFALanguage(languageId) not exist return nulptr
    */
    QFAEngineLanguage* ChangeLanguage(const char* languageId)
    {
        QFAEngineLanguage* lan = FindLanguage(languageId);
        if (!lan)
            return nullptr;

        Curentlanguage = lan;
        for (size_t i = 0; i < TextList.size(); i++)
            TextList[i]->Notify();

        return lan;
    }

    QFAEngineLanguage* FindLanguage(const char* languageId)
    {
        for (size_t i = 0; i < LanguageList.size(); i++)
            if (!strcmp(languageId, LanguageList[i]->Name))
                return LanguageList[i];

        return nullptr;
    }

    QFATextLocalization* GetText(const char* textId)
    {
        if (!Local)
            return nullptr;

        for (size_t i = 1; i < TextList.size(); i++)
            if (!strcmp(TextList[i]->Text.TextId, textId))
                return (QFATextLocalization*)TextList[i];

        return nullptr;
    }
};

class QFATextLocalization : public QFAEngineTextLocalization
{
    friend QFAClass;
    friend QFAText;


public:
    /*
        don't use directly instead LOCALTEXT
        only for defauld language
    */
    QFATextLocalization(const char* textId, const std::u32string& defaultText)
    {
        if (!textId)
            return;

        Text = { textId, defaultText };
        QFALocalization::GetLocalizationInstance()->AddTextInside(this);
    }

    /*      
        set/change text for languageId
        if language not exist ignor it ( need create by QFALocalization::AddLanguage) 
        if textId not exist ignor it ( need create by LOCALTEXT)
        if languageId is default language ignor it(eng)(default text can't be changed)
            SetText("eng", U"some text"); be ignored
    */
    void SetTextFor(const char* languageId, const std::u32string& text)
    {
        if (!strcmp(QFAEngineLanguage::DefaultName, languageId))
            return;

        QFALocalization* local = QFALocalization::GetLocalizationInstance();
        QFAEngineLanguage* lan = local->FindLanguage(languageId);
        if (!lan)
            return;
        
        bool itLanguageIdActive = false;
        if (local->Curentlanguage && !strcmp(local->Curentlanguage->Name, languageId))
            itLanguageIdActive = true;

        for (size_t i = 0; i < TextListOtherLanguage.size(); i++)
        {
            if (!strcmp(TextListOtherLanguage[i].languageId, languageId))
            {
                TextListOtherLanguage[i].text = text;
                if (itLanguageIdActive)
                    Notify();

                return;
            }
        }

        TextListOtherLanguage.push_back(STextOther{ languageId , text });
        if (itLanguageIdActive)
            Notify();
    }

    // return string for curent active language
    const std::u32string& GetCurentString() override
    {
        QFALocalization* local = QFALocalization::GetLocalizationInstance();
        if (!local->Curentlanguage)
        {
            static std::u32string tem(U"remove this text");
            return tem;
        }

        for (size_t i = 0; i < TextListOtherLanguage.size(); i++)
            if (!strcmp(TextListOtherLanguage[i].languageId, local->Curentlanguage->Name))
                return TextListOtherLanguage[i].text;

        return Text.Text;
    }
};
