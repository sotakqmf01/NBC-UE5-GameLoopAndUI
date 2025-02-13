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

	// 플레이어 컨트롤러도 마찬가지로 레벨이 생성될 때마다 생성되고, 레벨이 닫히면 사라짐
	// 다음 레벨이 되면 새로운 플레이어 컨트롤러가 만들어짐
	
	// PlayerController와 연결되어 있는 플레이어 가져오기
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// 해당 플레이어의 IMC를 관리하는 Subsystem을 가져오기
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// IMC를 할당
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
				CurrIMC = InputMappingContext;
			}
		}
	}
	
	// 에디터가 켜졌을 때 default map을 MenuLevel로 지정해놨음
	// => 게임 실행 시 첫 화면이 MainMenuUI가 뜸
	// => 만약 에디터에 켜놓은 map이 MenuLevel이 아니면 무시함
	// 맨 처음 화면은 Main Menu, 이후로는 무시될 거임
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu();
	}
}

// GameState에서 HUD 업데이트 할 때 필요
UUserWidget* AMyPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void AMyPlayerController::ShowMainMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Show Main Menu!!"));
	// 메뉴를 보여주려면 현재 보여지고 있는 위젯을 제거해야함
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

	// Main Menu 위젯 생성
	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		// 뷰포트에 렌더링
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			// 뒤쪽에 있는 게임 실행 화면은 그대로 있음
			// => 마우스 돌리면 백그라운드 화면 돌아감
			// 이걸 방지해줘야 함
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());

			MainMenuWidgetInstance->SetUserFocus(this);
		}
	}
}

void AMyPlayerController::ShowGameOverMenu(bool bIsClear)
{
	// 메뉴를 보여주려면 현재 보여지고 있는 위젯을 제거해야함
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

	// Game Over 위젯 생성
	if (GameOverWidgetClass)
	{
		GameOverWidgetInstance = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
		// 뷰포트에 렌더링
		if (GameOverWidgetInstance)
		{
			GameOverWidgetInstance->AddToViewport();

			// 뒤쪽에 있는 게임 실행 화면은 그대로 있음
			// => 마우스 돌리면 백그라운드 화면 돌아감
			// 이걸 방지해줘야 함
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());

			GameOverWidgetInstance->SetUserFocus(this);
		}

		// 클리어 여부에 따른 텍스트 출력
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

		// UI 애니메이션 실행
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
	// 메뉴를 보여주려면 현재 보여지고 있는 위젯을 제거해야함
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

	// HUD 위젯 생성
	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		// 뷰포트에 렌더링
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

// 버튼 클릭 이벤트에 바인딩될 함수
void AMyPlayerController::StartGame()
{
	// 게임 시작/재시작 시 전역 데이터 초기화
	if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		MyGameInstance->CurrentLevelIndex = 0;
		MyGameInstance->TotalScore = 0;
	}

	// 여기서도 OpenLevel()하면 GameState::BeginPlay()랑 충돌나는거 아닌가?
	// ▶ 아님 
	// ▶ OpenLevel()은 레벨 생성(전환) => GameState 생성 => GameState::BeginPlay() 호출 => StartLevel() 호출 = 레벨 초기화
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
		// Tab 키
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
		// 게임 플레이 중
		if (bIsTabMenuOff)
		{
			bIsTabMenuOff = false;

			// 백그라운드 화면 멈추기
			SetPause(true);

			// MenuInstance 없으니 생성
			if (TabMenuWidgetClass)
			{
				TabMenuWidgetInstance = CreateWidget<UUserWidget>(this, TabMenuWidgetClass);

				if (TabMenuWidgetInstance) {
					UTextBlock* ButtonText = Cast<UTextBlock>(TabMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText")));
					if (ButtonText) {
						ButtonText->SetText(FText::FromString(TEXT("Restart")));
					}

					UE_LOG(LogTemp, Warning, TEXT("Menu ON!"));
					// 뷰포트에 Menu 창 붙이기
					TabMenuWidgetInstance->AddToViewport();

					// 생성한 MenuInstance 
					// 마우스 커서 보여주기 + input 모드로 변경
					// ▶ input 모드로 하니까 마우스 클릭하려고 할 때 
					// ▶ UI창 한번 눌러서 focus 맞춰야 마우스 입력이 가능함
					// ▶ => 위에서 SetPause()하고 GameAndUI 모드로 설정
					bShowMouseCursor = true;
					SetInputMode(FInputModeGameAndUI().SetWidgetToFocus(TabMenuWidgetInstance->TakeWidget()));

					// Menu 위젯에서 키보드 입력 받기 위한 설정
					TabMenuWidgetInstance->bIsFocusable = true;
					// 위에서 SetPause(true)해서 키보드가 안먹히기 때문에
					// Menu 위젯이 키보드에 focus하도록 해야 입력 받을 수 있음
					TabMenuWidgetInstance->SetKeyboardFocus();
					//TabMenuWidgetInstance->OnKeyDown()
					//InKeyEvent.GetKey() == EKeys::Tab
				}
			}
		}
		else
		{
			// Menu창 켜져있을 때 tab 누르면 꺼지도록
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

// 게임 종료
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
//			// => 현재 적용된 IMC들이 담긴 TArray<FEnhancedActionKeyMapping>를 얻을 수 있음
//		}
//	}
//}