#include "EnemyCharacter.h"
#include "EnemyAIController.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// AIController 설정
	AIControllerClass = AEnemyAIController::StaticClass();

	// 레벨에 배치되거나 스폰되었을 때 AIController가 자동으로 빙의되도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}