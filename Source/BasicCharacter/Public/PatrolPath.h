// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class BASICCHARACTER_API APatrolPath : public AActor
{
	GENERATED_BODY()
	
public:	
	APatrolPath();

	// TargetPoint는 NavMesh 위에 있어야 제대로 작동함
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Patrol")
	TArray<AActor*> Waypoints;

	AActor* GetWaypoint(int32 index) const;
	int32 Num() const;

};