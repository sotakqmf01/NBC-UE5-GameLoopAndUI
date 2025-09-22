#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"


UCLASS()
class BASICCHARACTER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	// AIController가 AI 캐릭터에 빙의된 순간 호출되는 함수 - BeginPlay()보다 먼저 호출
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	// MoveToActor() 함수가 호출되어 목적지에 도달한 순간 호출되는 함수
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

protected:
	//int32 CurrentPatrolPointIndex = 0;
	//int32 Next = 1;

	//void MoveToCurrentPatrolPoint();
};
