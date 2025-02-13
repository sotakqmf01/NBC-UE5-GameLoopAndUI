// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class BASICCHARACTER_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnVolume();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;

public:
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomItem();

	FItemSpawnRow* GetRandomItem() const;
	FVector GetRandomPointInVolume() const;
	// TSubclassOf<T> : T를 포함하는 T의 하위 클래스들만 허용하는 템플릿 클래스
	//  - T클래스로 변환이 안되면 nullptr
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);
	
};
