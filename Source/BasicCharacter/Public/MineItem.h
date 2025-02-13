// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"


UCLASS()
class BASICCHARACTER_API AMineItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AMineItem();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Item|Component")
	USphereComponent* ExplosionCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ITem")
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ITem")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ITem")
	int32 ExplosionDamage;

	FTimerHandle ExplosionTimerHandle;

	bool bHasExploded;

public:
	virtual void ActivateItem(AActor* Activator) override;

	void Explode();
};
