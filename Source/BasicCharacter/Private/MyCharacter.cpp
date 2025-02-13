// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "MyPlayerController.h"
#include "MyGameState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Engine/GameEngine.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	//OverHeadWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidgetComp"));
	OverHeadWidgetHPBarComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidgetHPBarComp"));

	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	//OverHeadWidgetComp->SetupAttachment(GetMesh());
	OverHeadWidgetHPBarComp->SetupAttachment(GetMesh());

	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	SpringArmComp->bUsePawnControlRotation = true;		// Character�� �ƴ϶� [PlayerController�� ȸ��]�� �����ڴ�

	CameraComp->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	CameraComp->bUsePawnControlRotation = false;

	//OverHeadWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	OverHeadWidgetHPBarComp->SetWidgetSpace(EWidgetSpace::Screen);

	// Ŭ������ ã�ƾ� �ϴϱ� FObjectFinder<> ��� FClassFinder<> ���
	// Object�� �׳� ��� ������ ��µ� Class�� ��� ���� "_C"�� �߰��ؾ���
	// �� ���� �������Ʈ�� ��θ� ������ ���� �������� "_C"�� �ٿ���� �ν��� ����� �ϱ� ����
	//static ConstructorHelpers::FClassFinder<UUserWidget> WBP_HP(TEXT("/Game/UI/WBP_HP.WBP_HP_C"));
	//if (WBP_HP.Succeeded())
	//{
	//	OverHeadWidgetComp->SetWidgetClass(WBP_HP.Class);
	//}
	
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_HP(TEXT("/Game/UI/WBP_HPBar.WBP_HPBar_C"));
	if (WBP_HP.Succeeded())
	{
		OverHeadWidgetHPBarComp->SetWidgetClass(WBP_HP.Class);
	}

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100;
	Health = MaxHealth;

	MouseSensitivity = 0.25f;

	SlowDebuffStack = 0;

	// �±� �߰�
	Tags.Add(TEXT("Player"));
	Tags.Add(TEXT("JaeHoon"));
	Tags.Add("Unreal Engine");
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverheadHPBar();
	
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			// �̵�
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
			}
			// ����
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &AMyCharacter::StartJump);
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJump);
			}
			// ���콺
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
			}
			// �޸���
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &AMyCharacter::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &AMyCharacter::StopSprint);
			}
		}
	}
}

// FInputActionValue�� UEnhancedInputComponent �ȿ� ����
void AMyCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		// ���������� PawnMovementComponent�� �����Ǿ� �ִ� �Լ� ���
		// ĳ���͸� World �������� �̵���Ŵ
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AMyCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		// ���������� CharacterMovementComponent�� �����Ǿ� �ִ� �Լ� ���
		Jump();
	}
}

void AMyCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		// ���������� CharacterMovementComponent�� �����Ǿ� �ִ� �Լ� ���
		StopJumping();
	}
}

void AMyCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	// ���������� PlayerController�� �����Ǿ� �ִ� �Լ� ���
	AddControllerYawInput(LookInput.X * MouseSensitivity);
	AddControllerPitchInput(LookInput.Y * MouseSensitivity);
}

void AMyCharacter::StartSprint(const FInputActionValue& value)
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
}

void AMyCharacter::StopSprint(const FInputActionValue& value)
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

float AMyCharacter::GetHealth() const
{
	return Health;
}

void AMyCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);

	UpdateOverheadHPBar();
}

void AMyCharacter::DecreaseSpeed(float Slowness)
{
	NormalSpeed *= Slowness;
	SlowDebuffStack++;
}

void AMyCharacter::IncreaseSpeed(float Slowness)
{
	NormalSpeed /= Slowness;
	SlowDebuffStack--;
}

float AMyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// ActualDamage�� ����, ������ �̷��� ������ ������
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);

	UpdateOverheadHPBar();

	if (Health <= 0)
	{
		OnDeath();
		
	}

	return ActualDamage;
}

void AMyCharacter::OnDeath()
{
	AMyGameState* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
	if (MyGameState)
	{
		MyGameState->OnGameOver();
	}
}

void AMyCharacter::UpdateOverheadHPBar()
{
	if (!OverHeadWidgetHPBarComp) return;

	UUserWidget* OverHeadWidgetInstance = OverHeadWidgetHPBarComp->GetUserWidgetObject();
	if (!OverHeadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverHeadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}

	if (UProgressBar* HPBar = Cast<UProgressBar>(OverHeadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHPBar"))))
	{
		HPBar->SetPercent(FMath::Clamp(Health / MaxHealth, 0, 1));
	}
}