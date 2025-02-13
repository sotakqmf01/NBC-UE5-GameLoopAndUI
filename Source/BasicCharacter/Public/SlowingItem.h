// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowingItem.generated.h"

UCLASS()
class BASICCHARACTER_API ASlowingItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ASlowingItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slow")
	float Slowness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slow")
	float SlowTime;

protected:
	virtual void ActivateItem(AActor* Activator) override;
	
};
