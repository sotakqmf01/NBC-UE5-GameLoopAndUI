#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "Navigation/PathFollowingComponent.h"	// OnMoveCompleted()
//#include "NavAreas/NavArea_Jump.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 빙의했을 때 초기화, 준비 작업
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	//MoveToCurrentPatrolPoint();
}

// RequestID : 각 MoveToActor의 식별자, Result : MoveToActor의 결과값
void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	//if (Result.Code == EPathFollowingResult::Success)
	//{
	//	MoveToCurrentPatrolPoint();
	//}
}

//void AEnemyAIController::MoveToCurrentPatrolPoint()
//{
//	AEnemyCharacter* MyEnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
//	if (!MyEnemyCharacter)
//	{
//		return;
//	}
//
//	int32 PatrolPointCount = MyEnemyCharacter->PatrolPoints.Num();
//	if (PatrolPointCount == 0)
//	{
//		return;
//	}
//
//	// MoveToActor() : AIController에 구현되어 있는 함수
//	// - 최적의 이동 경로를 탐색하는 알고리즘이 들어 있음
//	MoveToActor(
//		MyEnemyCharacter->PatrolPoints[CurrentPatrolPointIndex],	// 목적지
//		10.0f,														// 
//		true,														// 캐릭터가 목표 지점에 도착하면 멈춤 설정
//		true,														// 장애물을 피할 것인지 설정
//		false,														// 좌/우 이동 없이 직진만 할 것인지 설정
//		nullptr,													// 필터
//		true														// 전체 경로를 탐색하지 못한 경우 부분 경로만이라도 이동할 것인지 설정
//	);
//	//UNavArea_Jump::StaticClass()
//	// 0→1→2→3→0→1→2
//	//CurrentPatrolPointIndex = (CurrentPatrolPointIndex + 1) % PatrolPointCount;
//
//	// 0→1→2→ 3 →2→1→0
//	if (CurrentPatrolPointIndex == PatrolPointCount - 1)
//	{
//		Next = -1;
//	}
//	else if(CurrentPatrolPointIndex == 0)
//	{
//		Next = 1;
//	}
//
//	CurrentPatrolPointIndex += Next;
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("index : %d"), CurrentPatrolPointIndex));
//}