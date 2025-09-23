#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class BASICCHARACTER_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	//UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
	//TArray<AActor*> PatrolPoints;
};