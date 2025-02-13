// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"
#include "MyCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20;
	ItemType = "HealingItem";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(Activator))
		{
			PlayerCharacter->AddHealth(HealAmount);
		}

		DestroyItem();
	}
}