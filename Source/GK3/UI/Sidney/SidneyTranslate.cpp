#include "SidneyTranslate.h"

#include "ActionManager.h"
#include "Actor.h"
#include "GameProgress.h"
#include "SidneyButton.h"
#include "SidneyFiles.h"
#include "SidneyPopup.h"
#include "SidneyUtil.h"
#include "Texture.h"
#include "UIImage.h"
#include "UINineSlice.h"
#include "UIUtil.h"

void SidneyTranslate::Init(Actor* parent, SidneyFiles* sidneyFiles)
{
    mSidneyFiles = sidneyFiles;

    // Add background. This will also be the root for this screen.
    mRoot = SidneyUtil::CreateBackground(parent);
    mRoot->SetName("Translate");

    // Add main menu button.
    SidneyUtil::CreateMainMenuButton(mRoot, [this](){
        Hide();
    });

    // Add menu bar.
    mMenuBar.Init(mRoot, SidneyUtil::GetTranslateLocalizer().GetText("ScreenName"), 140.0f);
    mMenuBar.SetFirstDropdownPosition(24.0f);
    mMenuBar.SetDropdownSpacing(26.0f);

    // "Open" dropdown.
    mMenuBar.AddDropdown(SidneyUtil::GetTranslateLocalizer().GetText("MenuName"));
    {
        // "Open File" choice.
        mMenuBar.AddDropdownChoice(SidneyUtil::GetTranslateLocalizer().GetText("MenuItem1"), [this](){

            // Show the file selector.
            mSidneyFiles->Show([this](SidneyFile* selectedFile){

                // The translate screen can only open text files.
                if(selectedFile->type != SidneyFileType::Text)
                {
                    mPopup->SetText(SidneyUtil::GetTranslateLocalizer().GetText("NotTranslatable"));
                    mPopup->ShowOneButton();
                }
                else
                {
                    OpenFile(selectedFile->id);
                }
            });
        });
    }
    
    // Create window for translation area.
    {
        mTranslateWindow = new Actor(TransformType::RectTransform);
        mTranslateWindow->GetTransform()->SetParent(mRoot->GetTransform());
        mTranslateWindow->GetComponent<RectTransform>()->SetAnchoredPosition(0.0f, -15.0f);

        UINineSlice* border = mTranslateWindow->AddComponent<UINineSlice>(SidneyUtil::GetGrayBoxParams(SidneyUtil::TransBgColor));
        border->GetRectTransform()->SetSizeDelta(526.0f, 340.0f);

        // Add divider line for box header.
        UIImage* headerDividerImage = UIUtil::NewUIActorWithWidget<UIImage>(mTranslateWindow);
        headerDividerImage->SetTexture(gAssetManager.LoadTexture("S_BOX_TOP.BMP"), true);
        headerDividerImage->GetRectTransform()->SetAnchor(AnchorPreset::TopStretch);
        headerDividerImage->GetRectTransform()->SetAnchoredPosition(0.0f, -17.0f);
        headerDividerImage->GetRectTransform()->SetSizeDeltaX(0.0f);

        // Add another one for the bottom section.
        UIImage* bottomDividerImage = UIUtil::NewUIActorWithWidget<UIImage>(mTranslateWindow);
        bottomDividerImage->SetTexture(gAssetManager.LoadTexture("S_BOX_TOP.BMP"), true);
        bottomDividerImage->GetRectTransform()->SetAnchor(AnchorPreset::TopStretch);
        bottomDividerImage->GetRectTransform()->SetAnchoredPosition(0.0f, -256.0f);
        bottomDividerImage->GetRectTransform()->SetSizeDeltaX(0.0f);
    }

    // Top part of window: header label in top right corner.
    {
        mTranslateHeaderLabel = UIUtil::NewUIActorWithWidget<UILabel>(mTranslateWindow);
        mTranslateHeaderLabel->SetFont(gAssetManager.LoadFont("SID_TEXT_14.FON"));
        mTranslateHeaderLabel->SetHorizonalAlignment(HorizontalAlignment::Right);
        mTranslateHeaderLabel->SetVerticalAlignment(VerticalAlignment::Top);
        mTranslateHeaderLabel->SetMasked(true);

        mTranslateHeaderLabel->GetRectTransform()->SetAnchor(AnchorPreset::TopRight);
        mTranslateHeaderLabel->GetRectTransform()->SetAnchoredPosition(-8.0f, -3.0f);
        mTranslateHeaderLabel->GetRectTransform()->SetSizeDeltaY(14.0f);
    }

    // Center part of window: area with text to be translated.
    {
        // Position black background to cover entire center area.
        UIImage* centerBackground = UIUtil::NewUIActorWithWidget<UIImage>(mTranslateWindow);
        centerBackground->SetTexture(&Texture::Black);
        centerBackground->GetRectTransform()->SetSizeDelta(524.0f, 237.0f);
        centerBackground->GetRectTransform()->SetAnchor(AnchorPreset::TopLeft);
        centerBackground->GetRectTransform()->SetAnchoredPosition(1, -18.0f);

        // Create text as child of background. Fill that area, with some margins on left/right.
        mTranslateTextLabel = UIUtil::NewUIActorWithWidget<UILabel>(centerBackground->GetOwner());
        mTranslateTextLabel->SetFont(gAssetManager.LoadFont("SID_TEXT_14.FON"));
        mTranslateTextLabel->SetText("** Open File: Arcad_Txt **\n\n\nEt in Arcadia Ego");
        mTranslateTextLabel->SetVerticalAlignment(VerticalAlignment::Top);

        mTranslateTextLabel->GetRectTransform()->SetAnchor(AnchorPreset::CenterStretch);
        mTranslateTextLabel->GetRectTransform()->SetSizeDelta(-16.0f, -16.0f);
        mTranslateTextLabel->GetRectTransform()->SetPixelPerfect(true);
    }

    // Create bottom part of window: translation options.
    {
        // Create static "From:" and "To:" labels.
        UILabel* fromLabel = UIUtil::NewUIActorWithWidget<UILabel>(mTranslateWindow);
        fromLabel->SetFont(gAssetManager.LoadFont("SID_PDN_10_UL.FON"));
        fromLabel->SetText(SidneyUtil::GetTranslateLocalizer().GetText("From"));
        fromLabel->SetVerticalAlignment(VerticalAlignment::Top);
        fromLabel->GetRectTransform()->SetAnchor(AnchorPreset::TopLeft);
        fromLabel->GetRectTransform()->SetAnchoredPosition(48.0f, -258.0f);
        fromLabel->GetRectTransform()->SetSizeDelta(100.0f, 10.0f);

        UILabel* toLabel = UIUtil::NewUIActorWithWidget<UILabel>(mTranslateWindow);
        toLabel->SetFont(gAssetManager.LoadFont("SID_PDN_10_UL.FON"));
        toLabel->SetText(SidneyUtil::GetTranslateLocalizer().GetText("To"));
        toLabel->SetVerticalAlignment(VerticalAlignment::Top);
        toLabel->GetRectTransform()->SetAnchor(AnchorPreset::TopLeft);
        toLabel->GetRectTransform()->SetAnchoredPosition(168.0f, -258.0f);
        toLabel->GetRectTransform()->SetSizeDelta(100.0f, 10.0f);

        // An image connecting the from and to buttons.
        UIImage* fromToImage = UIUtil::NewUIActorWithWidget<UIImage>(mTranslateWindow);
        fromToImage->SetTexture(gAssetManager.LoadTexture("S_FROM_TO.BMP"), true);
        fromToImage->GetRectTransform()->SetAnchor(AnchorPreset::TopLeft);
        fromToImage->GetRectTransform()->SetAnchoredPosition(105.0f, -271.0f);

        // Create from and to buttons for each of the supported languages.
        const float kFromX = 32.0f;
        const float kToX = 147.0f;
        float currY = -271.0f;
        const char* locKeys[] = { "English", "Latin", "French", "Italian" };
        for(int i = 0; i < 4; ++i)
        {
            Language language = static_cast<Language>(i);

            SidneyButton* fromButton = SidneyUtil::CreateSmallButton(mTranslateWindow);
            fromButton->SetWidth(70.0f);
            fromButton->SetText(StringUtil::ToUpperCopy(SidneyUtil::GetTranslateLocalizer().GetText(locKeys[i])));
            fromButton->GetRectTransform()->SetAnchor(AnchorPreset::TopLeft);
            fromButton->GetRectTransform()->SetAnchoredPosition(kFromX, currY);
            fromButton->SetPressCallback([this, fromButton, language](){
                mFromLanguage = language;
                for(SidneyButton* button : mFromButtons)
                {
                    if(button == fromButton)
                    {
                        button->SetSelected(true);
                    }
                    else
                    {
                        button->SetSelected(false);
                    }
                }
            });
            mFromButtons[i] = fromButton;

            SidneyButton* toButton = SidneyUtil::CreateSmallButton(mTranslateWindow);
            toButton->SetWidth(70.0f);
            toButton->SetText(StringUtil::ToUpperCopy(SidneyUtil::GetTranslateLocalizer().GetText(locKeys[i])));
            toButton->GetRectTransform()->SetAnchor(AnchorPreset::TopLeft);
            toButton->GetRectTransform()->SetAnchoredPosition(kToX, currY);
            toButton->SetPressCallback([this, toButton, language](){
                mToLanguage = language;
                for(SidneyButton* button : mToButtons)
                {
                    if(button == toButton)
                    {
                        button->SetSelected(true);
                    }
                    else
                    {
                        button->SetSelected(false);
                    }
                }
            });
            mToButtons[i] = toButton;

            currY -= 16.0f;
        }

        // Finally, a "translate now" button.
        SidneyButton* translateButton = SidneyUtil::CreateSmallButton(mTranslateWindow);
        translateButton->SetWidth(108.0f);
        translateButton->SetText(SidneyUtil::GetTranslateLocalizer().GetText("TranslateNow"));
        translateButton->GetRectTransform()->SetAnchor(AnchorPreset::TopLeft);
        translateButton->GetRectTransform()->SetAnchoredPosition(306.0f, -291.0f);
        translateButton->SetPressCallback([this](){
            OnTranslateButtonPressed();
        });
    }

    // Analyze message box.
    mPopup = new SidneyPopup(mRoot);
    mPopup->SetTextAlignment(HorizontalAlignment::Center);

    // Hide by default.
    Hide();
}

void SidneyTranslate::Show()
{
    mRoot->SetActive(true);
    OpenFile(mTranslateFileId);
}

void SidneyTranslate::Hide()
{
    mRoot->SetActive(false);
}

void SidneyTranslate::OnUpdate(float deltaTime)
{
    if(!mRoot->IsActive()) { return; }
    mMenuBar.Update();
}

void SidneyTranslate::OpenFile(int fileId)
{
    // Hide window by default.
    mTranslateWindow->SetActive(false);

    // Save file ID being translated.
    mTranslateFileId = fileId;

    // No file selected, so early out.
    if(fileId == -1) { return; }

    // Otherwise, show window and populate it.
    mTranslateWindow->SetActive(true);

    // Header shows current file name.
    std::string fileDisplayName = mSidneyFiles->GetFile(mTranslateFileId)->GetDisplayName();
    mTranslateHeaderLabel->SetText(fileDisplayName);

    // First line of body text should display that the file is opened.
    // This translation is assumed to be something like "Open File: %s" to accommodate formatting.
    std::string openFileLine = StringUtil::Format(SidneyUtil::GetTranslateLocalizer().GetText("OpenFile").c_str(), fileDisplayName.c_str());

    // Body text depends on the file being translated.
    std::string bodyText;
    if(mTranslateFileId == SidneyFileIds::kArcadiaText)
    {
        bodyText = SidneyUtil::GetTranslateLocalizer().GetText("ArcadiaText1");
    }

    // Populate translate text with combination of those texts.
    mTranslateTextLabel->SetText(openFileLine + "\n\n" + bodyText);

    // Reset from/to language selectors.
    mFromLanguage = Language::English;
    mToLanguage = Language::Latin;
    for(int i = 0; i < 4; ++i)
    {
        mFromButtons[i]->SetSelected(i == 0);
        mToButtons[i]->SetSelected(i == 1);
    }
}

void SidneyTranslate::OnTranslateButtonPressed()
{
    // Translating Arcadia text...
    if(mTranslateFileId == SidneyFileIds::kArcadiaText)
    {
        // "From" language must be Latin.
        if(mFromLanguage != Language::Latin)
        {
            mPopup->ResetToDefaults();
            mPopup->SetTextAlignment(HorizontalAlignment::Center);
            mPopup->SetText(SidneyUtil::GetTranslateLocalizer().GetText("WrongFrom"));
            mPopup->ShowOneButton();
            return;
        }

        // "To" language must be English.
        if(mToLanguage != Language::English)
        {
            // Grace says "in English please."
            gActionManager.ExecuteSheepAction("wait StartDialogue(\"02oc02zq91\", 1)");
            return;
        }

        // If we already translated it, we're done here.
        if(gGameProgress.GetFlag("ArcadiaComplete"))
        {
            mPopup->ResetToDefaults();
            mPopup->SetTextAlignment(HorizontalAlignment::Center);
            mPopup->SetText(SidneyUtil::GetTranslateLocalizer().GetText("NoFurther"));
            mPopup->ShowOneButton();
            return;
        }

        // Ok, translate Latin to English - that's what we want.
        // Add the translation to the output area the first time we try this.
        if(mTranslateTextLabel->GetLineCount() < 4)
        {
            std::string text = mTranslateTextLabel->GetText();
            text += "\n\n" + StringUtil::Format(SidneyUtil::GetTranslateLocalizer().GetText("Translating").c_str(), SidneyUtil::GetTranslateLocalizer().GetText("Latin").c_str());
            text += "\n\n" + SidneyUtil::GetTranslateLocalizer().GetText("ArcadiaTextT1");
            mTranslateTextLabel->SetText(text);
        }

        // We translated to/from the right languages, but the translation is missing a word.
        // Show "sentence incomplete - do you want to add words?" popup.
        mPopup->ResetToDefaults();
        mPopup->SetText(SidneyUtil::GetTranslateLocalizer().GetText("Subject") + "\n" + SidneyUtil::GetTranslateLocalizer().GetText("Question"));
        mPopup->SetTextAlignment(HorizontalAlignment::Left);
        mPopup->SetWindowPosition(Vector2(88.0f, 135.0f));
        mPopup->SetWindowSize(Vector2(280.0f, 120.0f));
        mPopup->ShowTwoButton([this](){
            // Show the UI flow (popups) to allow the player to enter the missing word.
            PromptForMissingWord();
        });
    }
}

void SidneyTranslate::PromptForMissingWord()
{
    // Show the popup that asks you to specify the word to use.
    mPopup->ResetToDefaults();
    mPopup->SetText(SidneyUtil::GetTranslateLocalizer().GetText("Input"));
    mPopup->SetWindowPosition(Vector2(88.0f, 148.0f));
    mPopup->SetWindowSize(Vector2(250.0f, 60.0f));
    mPopup->ShowTextInput([this](const std::string& input){

        // The password issssss.....sum!
        if(StringUtil::EqualsIgnoreCase(input, "sum"))
        {
            // The translation appends to the text output on screen.
            std::string text = mTranslateTextLabel->GetText();
            text += "\n\n" + SidneyUtil::GetTranslateLocalizer().GetText("ArcSUMText1");
            text += "\n\n" + StringUtil::Format(SidneyUtil::GetTranslateLocalizer().GetText("Translating").c_str(), SidneyUtil::GetTranslateLocalizer().GetText("Latin").c_str());
            text += "\n\n" + SidneyUtil::GetTranslateLocalizer().GetText("ArcSUMTextT1");
            text += "\n\n" + SidneyUtil::GetTranslateLocalizer().GetText("Updating");
            mTranslateTextLabel->SetText(text);

            // Grace says "BINGO we got it!"
            gActionManager.ExecuteSheepAction("wait StartDialogue(\"02OFS2ZPF5\", 1)");

            // We get some score and progress.
            gGameProgress.ChangeScore("e_sidney_translate_arcadia");
            gGameProgress.SetFlag("ArcadiaComplete");
        }
        else
        {
            // Wrong guess, would you like to try again? Y/N.
            mPopup->ResetToDefaults();
            mPopup->SetText(SidneyUtil::GetTranslateLocalizer().GetText("BadInput1") + "\n\n" + SidneyUtil::GetTranslateLocalizer().GetText("BadInput2"));
            mPopup->SetTextAlignment(HorizontalAlignment::Left);
            mPopup->SetWindowPosition(Vector2(88.0f, 135.0f));
            mPopup->SetWindowSize(Vector2(280.0f, 120.0f));
            mPopup->ShowTwoButton([this](){
                // Circle back around...
                PromptForMissingWord();
            });
        }
    });
}
