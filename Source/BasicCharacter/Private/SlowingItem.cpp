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
			// �ӵ� ����
			PlayerCharacter->DecreaseSpeed(Slowness);

			// ���ӽð��� ������ �� �̵��ӵ� ������ ���� �ڵ�
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

						// �������� ���ӽð��� �����ϱ� ���� ������ ���� TimerHandle�� ���µ�,
						// �� Ÿ�̸Ӱ� ���̳��� TimerHandle�� �ʿ䰡 �������� �޸� ���� �ϰ� �Ǵµ�
						// �̰� ��� ��� ó���ؾ��ұ ���� ����� �־��µ� �׳� �� ���� �Լ� ���� ó���ϸ� �Ǵ� ���̾���..
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


