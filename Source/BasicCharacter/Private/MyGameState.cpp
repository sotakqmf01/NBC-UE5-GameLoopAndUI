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

	// LoadMap ������ �߻��ϴ� ��� ���� ���μ����� ����� ����Ǿ� �ֽ��ϴ�.
	// GameMode, GameState, PlayerController ���� �ٸ� ��� �͵��� ������ �ε�� �Ŀ� �����Ǹ� �ش� �ʿ��� �÷����ϴ� ���ȿ��� �����˴ϴ�.
	// ��, �� ���� ���ų�, �ٸ� ������ �����ϰų� �⺻ �޴��� ���ư��� UWorld�� �����Ǹ� LoadMap�� ȣ��ɶ� ������� ��� ������Ʈ�� �����˴ϴ�.
	// https://openmynotepad.tistory.com/114
	// ����(��)�� ��ȯ�� �� ���� GameState�� ���� ������ => ������ �ʱ�ȭ => BeginPlay() ȣ���

	// ���� ������ ���� �����Ϳ� �����ִ� ���� �������� ����

	// Menu Level ������ ���� ȣ�� �� ����
	// HUD �����ִ� �� Ȯ��, start wave �α� �ߴ��� Ȯ��
	// => ShowGameHUD�� ���� ȣ��ǰ� ShowMainMenu()�� ȣ���, Wave �α׵� ��
	// ==> Menu Level�� �ƴ� ���� ���� �ʱ�ȭ �ϵ���
	FString CurrentMapName = GetWorld()->GetMapName();
	if (!CurrentMapName.Contains("MenuLevel"))
	{
		// ���� �ʱ�ȭ
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

// ���� ����
// �� ���� ���� �ƴ� - ���� ����(��ȯ)�� UGameplayStatics::OpenLevel() << �갡 �ϴ� ��
// ���� ������ �����ǰ��� StartLevel()�� ������ �ʱ�ȭ��Ŵ
void AMyGameState::StartLevel()
{
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// ���� ���� �� HUD UI�� ������
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			// �� ��ġ�� ������ Ÿ�̸� ���� ���� �ȵǾ� �־ ��� ����� �ȵǴ°� �ƴѰ�?
			// �� ��ġ�� �𸣰ڰ� StartLevel()���� �� �־�� ��
			// �� PlayerController�� BeginPlay()���� HUD UI�� �����ִ� �ڵ尡 ����
			// �� ���� �����Ϳ� MenuLevel�� �ƴ� �ٸ� ������ ���������� HUD UI�� �Ⱥ���
			// �� => ���� �ʱ�ȭ�ϴ� StartLevel()�� �־�� ��
			MyPlayerController->ShowGameHUD();
			MyPlayerController->bInGame = true;
		}
	}

	// ���� �����͸� ������ �ִ� GameInstance���� ���� level index ��������
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			CurrentLevelIndex = MyGameInstance->CurrentLevelIndex;
		}
	}

	StartWave();

	// ���� �ð� Ÿ�̸� ����
	GetWorldTimerManager().SetTimer(LevelTimerHandle, this, &AMyGameState::OnLevelTimeUp, LevelDuration, false);
}

void AMyGameState::StartWave()
{
	// ���س��� Wave �� ��ŭ�� ȣ��ǵ���
	if (WaveCount >= MaxWaveCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("START WAVE %d"), WaveCount);
	ShowWave();

	ItemToSpawn += (10 + CurrentLevelIndex) * WaveCount;

	// ������ ����
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				// ������ ����
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				// ������ �������� Coin ���������� Ȯ��
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
	// �ּ� Ŭ���� ���� �߰�
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
	// ���� ������ �Ѿ �� Ÿ�̸� �ʱ�ȭ
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

	// ������ ������ �����ϸ� ������ Game Over
	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		// ���� ���� ����(��ȯ) : ���� ���� ���ŵǰ� ���ο� ���� �� �ε�
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void AMyGameState::OnGameOver()
{
	// ���� ���� �� �޴� UI ������
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			// ���� ���� �� ��׶��忡 ���̴� ���� ȭ�� ����
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
				// [Time] �ؽ�Ʈ ����� �����ͼ� ��� �ؽ�Ʈ ����
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TimeText"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time : %.1f"), RemainingTime)));
				}

				// [Score] �ؽ�Ʈ ����� �����ͼ� ��� �ؽ�Ʈ ����
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

				// [GoalScore] �ؽ�Ʈ ����� �����ͼ� ��� �ؽ�Ʈ ����
				if (UTextBlock* GoalScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("GoalScoreText"))))
				{
					GoalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Clear : %d / %d"), Score, ClearScore)));

					if(Score >= ClearScore)
					{
						GoalScoreText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.2f)));
					}
				}

				// [Level] �ؽ�Ʈ ����� �����ͼ� ��� �ؽ�Ʈ ����
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("LevelText"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level : %d"), CurrentLevelIndex + 1)));
				}

				// [SlowStack] �ؽ�Ʈ ����� �����ͼ� ��� �ؽ�Ʈ ����
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
