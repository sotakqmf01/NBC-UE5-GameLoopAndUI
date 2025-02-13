// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowingItem.h"
#include "MyCharacter.h"
#include "MyGameState.h"
#include "Kismet/GameplayStatics.h"

ASlowingItem::ASlowingItem()
{
	ItemType = "Slow";
	Slowness = 0.5f;
	SlowTime = 4.0f;
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	//FName name = FName(GetName());

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(Activator))
		{
			PlayerCharacter->DecreaseSpeed(Slowness);

			TWeakObjectPtr<AMyCharacter> W_PlayerCharacter = PlayerCharacter;
			float SlownessForLambda = Slowness;

			if (AMyGameState* MyGameState = GetWorld()->GetGameState<AMyGameState>())
			{
				GetWorld()->GetTimerManager().SetTimer(
					MyGameState->SlowTimerHandle,
					[W_PlayerCharacter, SlownessForLambda]()
					{
						if (W_PlayerCharacter.IsValid())	W_PlayerCharacter->IncreaseSpeed(SlownessForLambda);
					},
					SlowTime,
					false);
				}
			}
			
		DestroyItem();
	}
}

