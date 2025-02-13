// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;	// IA���� ������ Value Type�� �ޱ� ���� ����ü

UCLASS()
class BASICCHARACTER_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

public:
	// �����
	int32 SlowDebuffStack;

protected:
	// ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	//UWidgetComponent* OverHeadWidgetComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverHeadWidgetHPBarComp;

	// �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float SprintSpeedMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float SprintSpeed;

	// ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	// ���콺 �ΰ���
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
		float DamageAmount,							// �������� : ������ ���ط�
		struct FDamageEvent const& DamageEvent,		// �������� ���� : �Ӽ�
		AController* EventInstigator,				// �������� ��ü : ���ڸ� ���� ���
		AActor* DamageCauser						// ������ ���� : ����
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
