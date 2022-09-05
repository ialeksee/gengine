#include "Sidney.h"

#include "ActionManager.h"
#include "LocationManager.h"
#include "Texture.h"
#include "UIButton.h"
#include "UICanvas.h"
#include "UIImage.h"
#include "UILabel.h"

UIButton* CreateMainButton(Actor* parent, const std::string& buttonId, float xPos)
{
    Actor* actor = new Actor(TransformType::RectTransform);
    actor->GetTransform()->SetParent(parent->GetTransform());
    UIButton* button = actor->AddComponent<UIButton>();

    button->GetRectTransform()->SetPivot(0.0f, 1.0f);
    button->GetRectTransform()->SetAnchor(0.0f, 1.0f);
    button->GetRectTransform()->SetAnchoredPosition(xPos, -24.0f);

    button->SetUpTexture(Services::GetAssets()->LoadTexture("B_" + buttonId + "_U.BMP"));
    button->SetHoverTexture(Services::GetAssets()->LoadTexture("B_" + buttonId + "_H.BMP"));
    button->SetDownTexture(Services::GetAssets()->LoadTexture("B_" + buttonId + "_D.BMP"));
    button->SetDisabledTexture(Services::GetAssets()->LoadTexture("B_" + buttonId + "_X.BMP"));
    return button;
}

Sidney::Sidney() : Actor(TransformType::RectTransform)
{
    // Sidney will be layered near the bottom.
    // A lot of stuff needs to appear above it (inventory, status overlay, etc).
    AddComponent<UICanvas>(-1);

    // Canvas takes up entire screen.
    RectTransform* rectTransform = GetComponent<RectTransform>();
    rectTransform->SetSizeDelta(0.0f, 0.0f);
    rectTransform->SetAnchorMin(Vector2::Zero);
    rectTransform->SetAnchorMax(Vector2::One);

    // Add black background that eats input.
    UIImage* background = AddComponent<UIImage>();
    background->SetTexture(&Texture::Black);
    background->SetReceivesInput(true);

    // Add desktop background image.
    Actor* desktopBackground = new Actor(TransformType::RectTransform);
    desktopBackground->GetTransform()->SetParent(GetTransform());
    UIImage* desktopBackgroundImage = desktopBackground->AddComponent<UIImage>();
    desktopBackgroundImage->SetTexture(Services::GetAssets()->LoadTexture("S_MAIN_SCN.BMP"), true);

    // Add exit button as child of desktop background.
    {
        Actor* exitButtonActor = new Actor(TransformType::RectTransform);
        exitButtonActor->GetTransform()->SetParent(desktopBackground->GetTransform());
        UIButton* exitButton = exitButtonActor->AddComponent<UIButton>();

        exitButton->GetRectTransform()->SetPivot(1.0f, 0.0f); // Bottom-Right
        exitButton->GetRectTransform()->SetAnchor(1.0f, 0.0f); // Bottom-Right
        exitButton->GetRectTransform()->SetAnchoredPosition(-10.0f, 10.0f); // 10x10 offset from Bottom-Right
        exitButton->GetRectTransform()->SetSizeDelta(80.0f, 18.0f);

        //TODO: Exit button uses a stretch-based image solution (kind of like 9-slice, more like 3-slice).
        exitButton->SetResizeBasedOnTexture(false);
        exitButton->SetUpTexture(&Texture::White);

        exitButton->SetPressCallback([this](UIButton* button){
            Services::GetAudio()->PlaySFX(Services::GetAssets()->LoadAudio("SIDBUTTON3.WAV"));
            Hide();
        });

        // Add exit button text.
        UILabel* exitLabel = exitButtonActor->AddComponent<UILabel>();
        exitLabel->SetFont(Services::GetAssets()->LoadFont("SID_TEXT_18.FON"));
        exitLabel->SetText("EXIT");
        exitLabel->SetHorizonalAlignment(HorizontalAlignment::Center);
        exitLabel->SetVerticalAlignment(VerticalAlignment::Center);
    }

    // Add button bar for subscreens.
    {
        const float kButtonStart = 5.0f;
        const float kButtonSpacing = 79.0f;

        float buttonPos = kButtonStart;
        UIButton* searchButton = CreateMainButton(desktopBackground, "SEARCH", buttonPos);
        searchButton->SetPressCallback([this](UIButton* button){
            Services::GetAudio()->PlaySFX(Services::GetAssets()->LoadAudio("SIDENTER.WAV"));
            mSearch.Show();
        });

        buttonPos += kButtonSpacing;
        UIButton* emailButton = CreateMainButton(desktopBackground, "EMAIL", buttonPos);
        emailButton->SetPressCallback([this](UIButton* button){
            Services::GetAudio()->PlaySFX(Services::GetAssets()->LoadAudio("SIDENTER.WAV"));
            mEmail.Show();
        });

        buttonPos += kButtonSpacing;
        UIButton* filesButton = CreateMainButton(desktopBackground, "FILES", buttonPos);
        filesButton->SetPressCallback([](UIButton* button){
            Services::GetAudio()->PlaySFX(Services::GetAssets()->LoadAudio("SIDENTER.WAV"));
            printf("Files\n");
        });

        buttonPos += kButtonSpacing;
        UIButton* analyzeButton = CreateMainButton(desktopBackground, "ANALYZE", buttonPos);
        analyzeButton->SetPressCallback([](UIButton* button){
            Services::GetAudio()->PlaySFX(Services::GetAssets()->LoadAudio("SIDENTER.WAV"));
            printf("Analyze\n");
        });

        buttonPos += kButtonSpacing;
        UIButton* translateButton = CreateMainButton(desktopBackground, "TRANSL", buttonPos);
        translateButton->SetPressCallback([](UIButton* button){
            Services::GetAudio()->PlaySFX(Services::GetAssets()->LoadAudio("SIDENTER.WAV"));
            printf("Translate\n");
        });

        buttonPos += kButtonSpacing;
        UIButton* dataButton = CreateMainButton(desktopBackground, "ADDATA", buttonPos);
        dataButton->SetPressCallback([](UIButton* button){
            Services::GetAudio()->PlaySFX(Services::GetAssets()->LoadAudio("SIDENTER.WAV"));

            //TODO: Grace says this line if you try to add data in 207A.
            //TODO: When does she stop saying this and let you add data? Is it based on the current timeblock?
            Services::Get<ActionManager>()->ExecuteSheepAction("wait StartDialogue(\"0264G2ZPF1\", 2)");
        });

        buttonPos += kButtonSpacing;
        UIButton* idButton = CreateMainButton(desktopBackground, "MAKEID", buttonPos);
        idButton->SetPressCallback([](UIButton* button){
            Services::GetAudio()->PlaySFX(Services::GetAssets()->LoadAudio("SIDENTER.WAV"));
            printf("Make ID\n");
        });

        buttonPos += kButtonSpacing;
        UIButton* suspectsButton = CreateMainButton(desktopBackground, "SUSPT", buttonPos);
        suspectsButton->SetPressCallback([](UIButton* button){
            Services::GetAudio()->PlaySFX(Services::GetAssets()->LoadAudio("SIDENTER.WAV"));
            printf("Suspects\n");
        });
    }

    // Add "New Email" label.
    {
        Actor* newEmailActor = new Actor(TransformType::RectTransform);
        newEmailActor->GetTransform()->SetParent(desktopBackground->GetTransform());
        mNewEmailLabel = newEmailActor->AddComponent<UILabel>();

        mNewEmailLabel->SetFont(Services::GetAssets()->LoadFont("SID_PDN_10_GRN.FON"));
        mNewEmailLabel->SetText("NEW E-MAIL");
        mNewEmailLabel->SetHorizonalAlignment(HorizontalAlignment::Right);
        mNewEmailLabel->SetVerticalAlignment(VerticalAlignment::Top);

        mNewEmailLabel->GetRectTransform()->SetPivot(1.0f, 1.0f);
        mNewEmailLabel->GetRectTransform()->SetAnchor(1.0f, 1.0f);
        mNewEmailLabel->GetRectTransform()->SetAnchoredPosition(0.0f, 0.0f);
        mNewEmailLabel->GetRectTransform()->SetSizeDelta(100.0f, 20.0f);
    }

    // Create subscreens.
    mSearch.Init(this);
    mEmail.Init(this);
}

void Sidney::Show()
{
    // Show Sidney UI.
    SetActive(true);

    //TODO: We'll assume there is ALWAYS new email for the moment...
    bool newEmail = true;

    // Hide the "new email" label by default, regardless of new email state.
    // If we DO have new email, this will blink on in a moment.
    mNewEmailLabel->SetEnabled(false);

    // If no new email, set this timer to -1. The label will never appear.
    // Otherwise, set to blink interval to have it blink on and off.
    mNewEmailBlinkTimer = newEmail ? kNewEmailBlinkInterval : -1;

    // If there is new email, play the "new email" audio cue.
    if(newEmail)
    {
        Services::GetAudio()->PlaySFX(Services::GetAssets()->LoadAudio("NEWEMAIL.WAV"));
    }
}

void Sidney::Hide()
{
    // Hide Sidney UI.
    SetActive(false);

    // Whenever you exit Sidney, no matter where you are in the game, you warp to R25.
    // This makes sense under the assumption that you only access Sidney in R25 anyway!
    Services::Get<LocationManager>()->ChangeLocation("R25", [](){

        // This special function warps Ego to the "sitting at desk" position and plays the stand up animation.
        Services::Get<ActionManager>()->ExecuteSheepAction("R25_ALL", "ExitSidney$");
    });
}

void Sidney::OnUpdate(float deltaTime)
{
    // Track timer countdown for new email to blink in the corner.
    if(mNewEmailBlinkTimer > 0.0f)
    {
        mNewEmailBlinkTimer -= deltaTime;
        if(mNewEmailBlinkTimer <= 0.0f)
        {
            mNewEmailLabel->SetEnabled(!mNewEmailLabel->IsEnabled());
            mNewEmailBlinkTimer = kNewEmailBlinkInterval;
        }
    }

    // Update each screen in turn.
    // Each screen will early out if not active.
    mSearch.OnUpdate(deltaTime);
}