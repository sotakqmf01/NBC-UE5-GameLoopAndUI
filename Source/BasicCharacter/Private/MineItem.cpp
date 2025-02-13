// Fill out your copyright notice in the Description page of Project Settings.


#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	ItemType = "Mine";
	ExplosionDelay = 5.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30;
	bHasExploded = false;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
	
}

// 계속 왔다갔다 하면서 overlap반복하면
// Timer가 덮어쓰여지고 덮어쓰여짐
// => overlap이 계속 trigger되면 폭발 안함
// ==> BaseItem의 OnItemOverlap() 함수를 
void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;

	Super::ActivateItem(Activator);

	// Explod()에 매개변수가 없어서 이렇게 사용 가능
	// 매개변수가 있으면 FTimerDelegate를 만들어서 사용해야 함
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay, false);

	bHasExploded = true;
}

void AMineItem::Explode()
{
	UE_LOG(LogTemp, Warning, TEXT("Explode()!!!!!!!!!!"));

	UParticleSystemComponent* Particle = nullptr;
	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), GetActorRotation(), false);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}

	// 폭발 데미지 입히기
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(Actor, ExplosionDamage, nullptr, this, UDamageType::StaticClass());
		}
	}

	DestroyItem();

	if (IsValid(Particle))
	{
		FTimerHandle DestroyParticleTimerHandle;

		TWeakObjectPtr<UPrimitiveComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(DestroyParticleTimerHandle, 
			[WeakParticle]()
			{
				if (WeakParticle.IsValid()) WeakParticle->DestroyComponent();
			},
			2.0f,
			false
		);
	}
}
