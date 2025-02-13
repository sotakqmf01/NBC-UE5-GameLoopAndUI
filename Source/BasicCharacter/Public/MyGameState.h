// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"


UCLASS()
class BASICCHARACTER_API AMyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AMyGameState();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 ClearScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 ItemToSpawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float WaveDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaveCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 WaveCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;		// 레벨map을 저장할 배열 

	// 타이머
	FTimerHandle LevelTimerHandle;
	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	FTimerHandle SlowTimerHandle;

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	void StartLevel();
	void StartWave();
	void OnLevelTimeUp();
	void OnCoinCollected();
	void EndLevel();
	void ShowWave();
	void UpdateHUD();
};
