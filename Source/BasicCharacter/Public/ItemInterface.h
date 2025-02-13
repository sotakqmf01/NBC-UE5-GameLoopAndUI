// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

class BASICCHARACTER_API IItemInterface
{
	GENERATED_BODY()

public:
	// ����, ����, ����
	// ����, ���� - ������ - ��� => (Ʈ����)
	// ���� - ������ - 5�� �� ���� - �������� �Ǿ������� - ������ => (������)
	
	// ������ �̺�Ʈ�� ���ε� �Ǵ� �Լ��� ������ �̺�Ʈ�� �Լ� �ñ״�ó(�Ű�����)�� �Ȱ��� ������ �־���Ѵ�
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,	// �� ������Ʈ �� overlap�� ������ component = SphereComponent
		AActor* OtherActor,						// SphereComponent�� �ε��� ����
		UPrimitiveComponent* OtherComp,			// �ε��� ������ component
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) = 0;
	UFUNCTION()
	virtual void OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
								  int32 OtherBodyIndex) = 0;

	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0;
};
