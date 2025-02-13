// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyGameState.h"
#include "MyGameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/GameEngine.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TextBlock.h"

AMyPlayerController::AMyPlayerController()
	: InputMappingContext(nullptr),
	TabInputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	TapKeyAction(nullptr),
	CurrIMC(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr),
	TabMenuWidgetClass(nullptr),
	TabMenuWidgetInstance(nullptr),
	GameOverWidgetClass(nullptr),
	GameOverWidgetInstance(nullptr),
	bIsTabMenuOff(true),
	bInGame(false)
{
	
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// �÷��̾� ��Ʈ�ѷ��� ���������� ������ ������ ������ �����ǰ�, ������ ������ �����
	// ���� ������ �Ǹ� ���ο� �÷��̾� ��Ʈ�ѷ��� �������
	
	// PlayerController�� ����Ǿ� �ִ� �÷��̾� ��������
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// �ش� �÷��̾��� IMC�� �����ϴ� Subsystem�� ��������
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// IMC�� �Ҵ�
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
				CurrIMC = InputMappingContext;
			}
		}
	}
	
	// �����Ͱ� ������ �� default map�� MenuLevel�� �����س���
	// => ���� ���� �� ù ȭ���� MainMenuUI�� ��
	// => ���� �����Ϳ� �ѳ��� map�� MenuLevel�� �ƴϸ� ������
	// �� ó�� ȭ���� Main Menu, ���ķδ� ���õ� ����
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu();
	}
}

// GameState���� HUD ������Ʈ �� �� �ʿ�
UUserWidget* AMyPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void AMyPlayerController::ShowMainMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Show Main Menu!!"));
	// �޴��� �����ַ��� ���� �������� �ִ� ������ �����ؾ���
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (GameOverWidgetInstance)
	{
		GameOverWidgetInstance->RemoveFromParent();
		GameOverWidgetInstance = nullptr;
	}

	// Main Menu ���� ����
	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		// ����Ʈ�� ������
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			// ���ʿ� �ִ� ���� ���� ȭ���� �״�� ����
			// => ���콺 ������ ��׶��� ȭ�� ���ư�
			// �̰� ��������� ��
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());

			MainMenuWidgetInstance->SetUserFocus(this);
		}
	}
}

void AMyPlayerController::ShowGameOverMenu(bool bIsClear)
{
	// �޴��� �����ַ��� ���� �������� �ִ� ������ �����ؾ���
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (GameOverWidgetInstance)
	{
		GameOverWidgetInstance->RemoveFromParent();
		GameOverWidgetInstance = nullptr;
	}

	// Game Over ���� ����
	if (GameOverWidgetClass)
	{
		GameOverWidgetInstance = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
		// ����Ʈ�� ������
		if (GameOverWidgetInstance)
		{
			GameOverWidgetInstance->AddToViewport();

			// ���ʿ� �ִ� ���� ���� ȭ���� �״�� ����
			// => ���콺 ������ ��׶��� ȭ�� ���ư�
			// �̰� ��������� ��
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());

			GameOverWidgetInstance->SetUserFocus(this);
		}

		// Ŭ���� ���ο� ���� �ؽ�Ʈ ���
		if (bIsClear)
		{
			if (UTextBlock* GameOverText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("GameOverText"))))
			{
				GameOverText->SetText(FText::FromString(TEXT("Clear!!")));
			}
		}
		else
		{
			if (UTextBlock* GameOverText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("GameOverText"))))
			{
				GameOverText->SetText(FText::FromString(TEXT("Game Over...")));
			}
		}

		// UI �ִϸ��̼� ����
		UFunction* PlayAnimFunc = GameOverWidgetInstance->FindFunction(FName("PlayGameOverAnim"));

		if (PlayAnimFunc)
		{
			GameOverWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
		}

		if (UTextBlock* TotalScoreText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("TotalScoreText"))))
		{
			//if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
			if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GetGameInstance()))
			{
				TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total Score : %d"), MyGameInstance->TotalScore)));
			}
		}
	}
}

void AMyPlayerController::ShowGameHUD()
{
	UE_LOG(LogTemp, Warning, TEXT("Show GameHUD"));
	// �޴��� �����ַ��� ���� �������� �ִ� ������ �����ؾ���
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	// HUD ���� ����
	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		// ����Ʈ�� ������
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
			bIsTabMenuOff = true;

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		AMyGameState* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
		if (MyGameState)
		{
			MyGameState->UpdateHUD();
		}
	}
}

// ��ư Ŭ�� �̺�Ʈ�� ���ε��� �Լ�
void AMyPlayerController::StartGame()
{
	// ���� ����/����� �� ���� ������ �ʱ�ȭ
	if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		MyGameInstance->CurrentLevelIndex = 0;
		MyGameInstance->TotalScore = 0;
	}

	// ���⼭�� OpenLevel()�ϸ� GameState::BeginPlay()�� �浹���°� �ƴѰ�?
	// �� �ƴ� 
	// �� OpenLevel()�� ���� ����(��ȯ) => GameState ���� => GameState::BeginPlay() ȣ�� => StartLevel() ȣ�� = ���� �ʱ�ȭ
	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	//SetPause(false);
}

void AMyPlayerController::StartMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("MenuLevel"));
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Tab Ű
		if (TapKeyAction)
		{
			EnhancedInput->BindAction(TapKeyAction, ETriggerEvent::Started, this, &AMyPlayerController::TabMenu);
		}
	}
}

void AMyPlayerController::TabMenu(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Tab Menu!!!!!!!!!!!!!!"));

	if (bInGame) {
		// ���� �÷��� ��
		if (bIsTabMenuOff)
		{
			bIsTabMenuOff = false;

			// ��׶��� ȭ�� ���߱�
			SetPause(true);

			// MenuInstance ������ ����
			if (TabMenuWidgetClass)
			{
				TabMenuWidgetInstance = CreateWidget<UUserWidget>(this, TabMenuWidgetClass);

				if (TabMenuWidgetInstance) {
					UTextBlock* ButtonText = Cast<UTextBlock>(TabMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText")));
					if (ButtonText) {
						ButtonText->SetText(FText::FromString(TEXT("Restart")));
					}

					UE_LOG(LogTemp, Warning, TEXT("Menu ON!"));
					// ����Ʈ�� Menu â ���̱�
					TabMenuWidgetInstance->AddToViewport();

					// ������ MenuInstance 
					// ���콺 Ŀ�� �����ֱ� + input ���� ����
					// �� input ���� �ϴϱ� ���콺 Ŭ���Ϸ��� �� �� 
					// �� UIâ �ѹ� ������ focus ����� ���콺 �Է��� ������
					// �� => ������ SetPause()�ϰ� GameAndUI ���� ����
					bShowMouseCursor = true;
					SetInputMode(FInputModeGameAndUI().SetWidgetToFocus(TabMenuWidgetInstance->TakeWidget()));

					// Menu �������� Ű���� �Է� �ޱ� ���� ����
					TabMenuWidgetInstance->bIsFocusable = true;
					// ������ SetPause(true)�ؼ� Ű���尡 �ȸ����� ������
					// Menu ������ Ű���忡 focus�ϵ��� �ؾ� �Է� ���� �� ����
					TabMenuWidgetInstance->SetKeyboardFocus();
					//TabMenuWidgetInstance->OnKeyDown()
					//InKeyEvent.GetKey() == EKeys::Tab
				}
			}
		}
		else
		{
			// Menuâ �������� �� tab ������ ��������
			bIsTabMenuOff = true;

			UE_LOG(LogTemp, Warning, TEXT("Menu OFF!"));
			TabMenuWidgetInstance->RemoveFromParent();
			TabMenuWidgetInstance = nullptr;

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());

			SetPause(false);
		}
	}
}

// ���� ����
void AMyPlayerController::ExitGame()
{
	//  bIgnorePlatformRestrictions = true :
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
}

//void AMyPlayerController::ChangeIMC(const FInputActionValue& value)
//{
//	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, TEXT("Change IMC"));
//
//	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
//	{
//		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
//		{
//			if (CurrIMC != TabInputMappingContext)
//			{
//				Subsystem->RemoveMappingContext(InputMappingContext);
//				Subsystem->AddMappingContext(TabInputMappingContext, 1);
//				CurrIMC = TabInputMappingContext;
//				GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Blue, TEXT("TabInputMappingContext IMC"));
//			}
//			else
//			{
//				Subsystem->RemoveMappingContext(TabInputMappingContext);
//				Subsystem->AddMappingContext(InputMappingContext, 0);
//				CurrIMC = InputMappingContext;
//				GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Blue, TEXT("InputMappingContext IMC"));
//			}
//
//			// UEnhancedInputLocalPlayerSubsystem -> GetPlayerInput() -> UEnhancedPlayerInput -> GetEnhancedActionMappings()
//			// => ���� ����� IMC���� ��� TArray<FEnhancedActionKeyMapping>�� ���� �� ����
//		}
//	}
//}