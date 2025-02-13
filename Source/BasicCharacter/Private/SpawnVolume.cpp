// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);
	
	ItemDataTable = nullptr;
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Game/DataTables/BasicLevelSpawnTable.BasicLevelSpawnTable"));
	if (DataTable.Succeeded())
	{
		ItemDataTable = DataTable.Object;
	}
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		// 랜덤하게 뽑은 데이터를 보고 해당 아이템의 메타 데이터를 생성
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}

	return nullptr;
}

// 아이템 데이터 테이블에서 랜덤한 아이템 데이터 뽑기
FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	// 데이터 테이블이 없는 경우
	if (!ItemDataTable) return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));	// 디버깅에 사용되는 문자열, GetAllRows()를 쓸 때 필요

	ItemDataTable->GetAllRows(ContextString, AllRows);

	// 데이터 테이블에 데이터가 없는 경우
	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	// 누적 확률 랜덤 뽑기
	const float RandomValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandomValue <= AccumulateChance)
		{
			return Row;
		}
	}

	return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	// 기본 크기(200, 100, 50) * Scale(3, 2, 1) => (600, 200, 50)
	// GetScaledBoxExtent()는 스케일이 적용된 박스 크기의 절반 크기를 반환 => (300, 100, 25)
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	// 위치 = 중심
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
								FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
								FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z));
}

// 넘겨받은 메타데이터를 사용해서 객체 인스턴스 생성 
// => 동적으로 클래스를 생성할 수 있다는게 이 부분 같음
// ▶ 실행(Runtime) 중에 어떤 객체의 클래스 메타데이터를 보고 - SpawnRandomItem()
// ▶ 해당 객체 인스턴스를 생성 - SpawnItem(TSubclassOf<AActor> ItemClass)
AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Spawn Item")));

	return GetWorld()->SpawnActor<AActor>(ItemClass, GetRandomPointInVolume(), FRotator::ZeroRotator);
}