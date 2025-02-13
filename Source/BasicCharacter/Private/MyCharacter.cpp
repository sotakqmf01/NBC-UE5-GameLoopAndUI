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
	SpringArmComp->bUsePawnControlRotation = true;		// Character가 아니라 [PlayerController의 회전]을 따르겠다

	CameraComp->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	CameraComp->bUsePawnControlRotation = false;

	//OverHeadWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	OverHeadWidgetHPBarComp->SetWidgetSpace(EWidgetSpace::Screen);

	// 클래스를 찾아야 하니까 FObjectFinder<> 대신 FClassFinder<> 사용
	// Object는 그냥 경로 넣으면 됬는데 Class는 경로 끝에 "_C"를 추가해야함
	// ▶ ㄴㄴ 블루프린트의 경로를 가져올 때는 마지막에 "_C"를 붙여줘야 인식을 제대로 하기 때문
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

	// 태그 추가
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
			// 이동
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
			}
			// 점프
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &AMyCharacter::StartJump);
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJump);
			}
			// 마우스
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
			}
			// 달리기
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &AMyCharacter::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &AMyCharacter::StopSprint);
			}
		}
	}
}

// FInputActionValue는 UEnhancedInputComponent 안에 있음
void AMyCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		// 내부적으로 PawnMovementComponent에 구현되어 있는 함수 사용
		// 캐릭터를 World 기준으로 이동시킴
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
		// 내부적으로 CharacterMovementComponent에 구현되어 있는 함수 사용
		Jump();
	}
}

void AMyCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		// 내부적으로 CharacterMovementComponent에 구현되어 있는 함수 사용
		StopJumping();
	}
}

void AMyCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	// 내부적으로 PlayerController에 구현되어 있는 함수 사용
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
	// ActualDamage는 방어력, 내구력 이런거 적용한 데미지
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