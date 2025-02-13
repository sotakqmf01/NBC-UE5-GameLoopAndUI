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

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(Activator))
		{
			// 속도 감소
			PlayerCharacter->DecreaseSpeed(Slowness);

			// 지속시간이 지났을 때 이동속도 복구를 위한 코드
			TWeakObjectPtr<AMyCharacter> W_PlayerCharacter = PlayerCharacter;
			float SlownessForLambda = Slowness;

			if (AMyGameState* MyGameState = GetWorld()->GetGameState<AMyGameState>())
			{
				FName ItemName = FName(GetName());
				MyGameState->GetMostRecentTimerHandle(ItemName);

				FTimerHandle& SlowTimerHandle = MyGameState->SlowTimerHandles[ItemName];

				TWeakObjectPtr<AMyGameState> W_MyGameState = MyGameState;

				GetWorld()->GetTimerManager().SetTimer(
					SlowTimerHandle,
					[W_PlayerCharacter, SlownessForLambda, W_MyGameState, ItemName]()
					{
						if (W_PlayerCharacter.IsValid())
						{
							W_PlayerCharacter->IncreaseSpeed(SlownessForLambda);
						}

						// 아이템의 지속시간을 추적하기 위해 아이템 마다 TimerHandle를 쓰는데,
						// 이 타이머가 끝이나면 TimerHandle이 필요가 없어지고 메모리 낭비를 하게 되는데
						// 이걸 어디서 어떻게 처리해야할까에 대한 고민이 있었는데 그냥 이 람다 함수 에서 처리하면 되는 것이었다..
						if (W_MyGameState.IsValid())
						{
							W_MyGameState->SlowTimerHandles.Remove(ItemName);
						}
					},
					SlowTime,
					false);
				}
			}
			
		DestroyItem();
	}
}


