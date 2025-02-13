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
		// �����ϰ� ���� �����͸� ���� �ش� �������� ��Ÿ �����͸� ����
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}

	return nullptr;
}

// ������ ������ ���̺��� ������ ������ ������ �̱�
FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	// ������ ���̺��� ���� ���
	if (!ItemDataTable) return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));	// ����뿡 ���Ǵ� ���ڿ�, GetAllRows()�� �� �� �ʿ�

	ItemDataTable->GetAllRows(ContextString, AllRows);

	// ������ ���̺� �����Ͱ� ���� ���
	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	// ���� Ȯ�� ���� �̱�
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
	// �⺻ ũ��(200, 100, 50) * Scale(3, 2, 1) => (600, 200, 50)
	// GetScaledBoxExtent()�� �������� ����� �ڽ� ũ���� ���� ũ�⸦ ��ȯ => (300, 100, 25)
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	// ��ġ = �߽�
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
								FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
								FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z));
}

// �Ѱܹ��� ��Ÿ�����͸� ����ؼ� ��ü �ν��Ͻ� ���� 
// => �������� Ŭ������ ������ �� �ִٴ°� �� �κ� ����
// �� ����(Runtime) �߿� � ��ü�� Ŭ���� ��Ÿ�����͸� ���� - SpawnRandomItem()
// �� �ش� ��ü �ν��Ͻ��� ���� - SpawnItem(TSubclassOf<AActor> ItemClass)
AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Spawn Item")));

	return GetWorld()->SpawnActor<AActor>(ItemClass, GetRandomPointInVolume(), FRotator::ZeroRotator);
}