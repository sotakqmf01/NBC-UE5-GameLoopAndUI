// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"
#include "MyGameInstance.h"
#include "MyPlayerController.h"
#include "MyCharacter.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SlowingItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

AMyGameState::AMyGameState()
{
	Score = 0;
	ClearScore = 300;
	ItemToSpawn = 20;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	WaveDuration = 10.0f;
	MaxWaveCount = 3;
	WaveCount = 0;
	LevelDuration = WaveDuration * MaxWaveCount + 0.2f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	
	RecentlyActivatedSlowingItemName = TEXT("");
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();

	// LoadMap 이전에 발생하는 모든 일은 프로세스의 수명과 연결되어 있습니다.
	// GameMode, GameState, PlayerController 같은 다른 모든 것들은 지도가 로드된 후에 생성되며 해당 맵에서 플레이하는 동안에만 유지됩니다.
	// 즉, 새 맵을 열거나, 다른 서버에 연결하거나 기본 메뉴로 돌아가면 UWorld는 정리되며 LoadMap이 호출될때 만들어진 모든 오브젝트가 삭제됩니다.
	// https://openmynotepad.tistory.com/114
	// 레벨(맵)이 전환될 때 마다 GameState는 새로 생성됨 => 변수들 초기화 => BeginPlay() 호출됨

	// 게임 시작은 현재 에디터에 켜져있는 레벨 기준으로 시작

	// Menu Level 열였을 때도 호출 될 거임
	// HUD 보여주는 지 확인, start wave 로그 뜨는지 확인
	// => ShowGameHUD가 먼저 호출되고 ShowMainMenu()가 호출됨, Wave 로그도 뜸
	// ==> Menu Level이 아닐 때만 레벨 초기화 하도록
	FString CurrentMapName = GetWorld()->GetMapName();
	if (!CurrentMapName.Contains("MenuLevel"))
	{
		// 레벨 초기화
		StartLevel();

		GetWorldTimerManager().SetTimer(HUDUpdateTimerHandle, this, &AMyGameState::UpdateHUD, 0.1f, true);
	}
}

int32 AMyGameState::GetScore() const
{
	return Score;
}

void AMyGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			Score += Amount;
			MyGameInstance->AddToScore(Amount);
		}
	}
}

// 레벨 생성
// ▶ 레벨 생성 아님 - 레벨 생성(전환)은 UGameplayStatics::OpenLevel() << 얘가 하는 것
// ▶▶ 레벨을 생성되고나면 StartLevel()은 레벨을 초기화시킴
void AMyGameState::StartLevel()
{
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// 레벨 오픈 시 HUD UI를 보여줌
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			// 이 위치에 있으면 타이머 설정 아직 안되어 있어서 출력 제대로 안되는거 아닌가?
			// ▷ 위치는 모르겠고 StartLevel()에는 꼭 있어야 함
			// ▷ PlayerController의 BeginPlay()에는 HUD UI를 보여주는 코드가 없음
			// ▷ 만약 에디터에 MenuLevel이 아닌 다른 레벨이 열려있으면 HUD UI가 안보임
			// ▷ => 레벨 초기화하는 StartLevel()에 있어야 함
			MyPlayerController->ShowGameHUD();
			MyPlayerController->bInGame = true;
		}
	}

	// 전역 데이터를 가지고 있는 GameInstance에서 현재 level index 가져오기
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			CurrentLevelIndex = MyGameInstance->CurrentLevelIndex;
		}
	}

	StartWave();

	// 제한 시간 타이머 시작
	GetWorldTimerManager().SetTimer(LevelTimerHandle, this, &AMyGameState::OnLevelTimeUp, LevelDuration, false);
}

void AMyGameState::StartWave()
{
	// 정해놓은 Wave 수 만큼만 호출되도록
	if (WaveCount >= MaxWaveCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("START WAVE %d"), WaveCount);
	ShowWave();

	ItemToSpawn += (10 + CurrentLevelIndex) * WaveCount;

	// 아이템 스폰
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				// 아이템 스폰
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				// 스폰한 아이템이 Coin 아이템인지 확인
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	WaveCount++;

	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AMyGameState::StartWave, WaveDuration, false);
}

void AMyGameState::OnLevelTimeUp()
{
	// 최소 클리어 점수 추가
	if (Score >= ClearScore)
	{
		EndLevel();
	}
	else
	{
		OnGameOver();
	}
}

void AMyGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected : %d / %d"), CollectedCoinCount, SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndLevel();
	}
}

void AMyGameState::EndLevel()
{
	// 다음 레벨로 넘어갈 때 타이머 초기화
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	CurrentLevelIndex++;

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			MyGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	// 마지막 레벨에 도착하면 다음은 Game Over
	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		// 다음 레벨 생성(전환) : 기존 레벨 제거되고 새로운 레벨 맵 로드
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void AMyGameState::OnGameOver()
{
	// 게임 오버 시 메뉴 UI 보여줌
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			// 게임 오버 시 백그라운드에 보이는 게임 화면 정지
			MyPlayerController->SetPause(true);
			if (Score >= ClearScore)
			{
				// Clear
				MyPlayerController->ShowGameOverMenu(true);
			}
			else
			{
				// Game Over
				MyPlayerController->ShowGameOverMenu(false);
			}
			
		}
	}
}

void AMyGameState::ShowWave()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = MyPlayerController->GetHUDWidget())
			{
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveText"))))
				{
					UE_LOG(LogTemp, Warning, TEXT("Wave Text Update!"));
					WaveText->SetText(FText::FromString(FString::Printf(TEXT(" - Wave %d -"), WaveCount + 1)));
				}

				if (UFunction* PlayAnimFunc = HUDWidget->FindFunction(FName("PlayWaveAnim")))
				{
					UE_LOG(LogTemp, Warning, TEXT("Wave Text Animation Start!"));
					HUDWidget->ProcessEvent(PlayAnimFunc, nullptr);
				}
			}
		}
	}
}

void AMyGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			// WBP_HUD
			if (UUserWidget* HUDWidget = MyPlayerController->GetHUDWidget())
			{
				// [Time] 텍스트 블록을 가져와서 출력 텍스트 설정
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TimeText"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time : %.1f"), RemainingTime)));
				}

				// [Score] 텍스트 블록을 가져와서 출력 텍스트 설정
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("ScoreText"))))
				{
					if(UGameInstance* GameInstance = GetGameInstance())
					{
						UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
						if (MyGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), MyGameInstance->TotalScore)));
						}
					}
				}

				// [GoalScore] 텍스트 블록을 가져와서 출력 텍스트 설정
				if (UTextBlock* GoalScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("GoalScoreText"))))
				{
					GoalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Clear : %d / %d"), Score, ClearScore)));

					if(Score >= ClearScore)
					{
						GoalScoreText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.2f)));
					}
				}

				// [Level] 텍스트 블록을 가져와서 출력 텍스트 설정
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("LevelText"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level : %d"), CurrentLevelIndex + 1)));
				}

				// [SlowStack] 텍스트 블록을 가져와서 출력 텍스트 설정
				if (UTextBlock* SlowStackText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("SlowStackText"))))
				{
					if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
					{
						float RemainingTime = 0.0f;

						if (SlowTimerHandles.Num() > 0)
						{
							RemainingTime = GetWorldTimerManager().GetTimerRemaining(SlowTimerHandles[RecentlyActivatedSlowingItemName]);
							if (RemainingTime < 0.0f)
							{
								RemainingTime = 0.0f;
							}
						}

						if (PlayerCharacter->SlowDebuffStack <= 0)
						{
							SlowStackText->SetVisibility(ESlateVisibility::Hidden);
						}
						else
						{
							SlowStackText->SetVisibility(ESlateVisibility::Visible);
						}
						

						SlowStackText->SetText(FText::FromString(FString::Printf(TEXT("Slow x %d (%.1f)"), PlayerCharacter->SlowDebuffStack, RemainingTime)));
					}
				}
			}
		}
	}
}

void AMyGameState::GetMostRecentTimerHandle(FName ItemName)
{
	RecentlyActivatedSlowingItemName = ItemName;
	SlowTimerHandles.Add(ItemName);
}
