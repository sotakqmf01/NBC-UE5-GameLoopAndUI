// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;	// IA에서 설정한 Value Type을 받기 위한 구조체

UCLASS()
class BASICCHARACTER_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

public:
	// 디버프
	int32 SlowDebuffStack;

protected:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	//UWidgetComponent* OverHeadWidgetComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverHeadWidgetHPBarComp;

	// 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float SprintSpeedMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float SprintSpeed;

	// 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	// 마우스 민감도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse")
	float MouseSensitivity;

public:
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);
	UFUNCTION(BlueprintCallable, Category = "Move")
	void DecreaseSpeed(float Slowness);
	UFUNCTION(BlueprintCallable, Category = "Move")
	void IncreaseSpeed(float Slowness);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount,							// 데미지량 : 지뢰의 피해량
		struct FDamageEvent const& DamageEvent,		// 데미지의 유형 : 속성
		AController* EventInstigator,				// 데미지의 주체 : 지뢰를 심은 사람
		AActor* DamageCauser						// 데미지 원인 : 지뢰
	) override;				

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	void OnDeath();
	//void UpdateOverheadHP();
	void UpdateOverheadHPBar();

};
