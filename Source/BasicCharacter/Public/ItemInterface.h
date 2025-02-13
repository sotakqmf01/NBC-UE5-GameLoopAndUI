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
	// 지뢰, 힐링, 코인
	// 힐링, 코인 - 오버랩 - 즉발 => (트리거)
	// 지뢰 - 오버랩 - 5초 뒤 폭발 - 오버랩이 되어있으면 - 데미지 => (오버랩)
	
	// 오버랩 이벤트에 바인딩 되는 함수는 오버랩 이벤트의 함수 시그니처(매개변수)를 똑같이 가지고 있어야한다
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,	// 내 컴포넌트 중 overlap을 감지한 component = SphereComponent
		AActor* OtherActor,						// SphereComponent에 부딪힌 액터
		UPrimitiveComponent* OtherComp,			// 부딪힌 액터의 component
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) = 0;
	UFUNCTION()
	virtual void OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
								  int32 OtherBodyIndex) = 0;

	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0;
};
