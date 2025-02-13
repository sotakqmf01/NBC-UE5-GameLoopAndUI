// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereCollision->SetupAttachment(Scene);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SphereCollision);

	// 이벤트 바인딩
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverlap);
}

void ABaseItem::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// tag는 닉네임 같은 것
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Overlap!")));
		ActivateItem(OtherActor);
	}
}

void ABaseItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABaseItem::ActivateItem(AActor* Activator)
{
	UE_LOG(LogTemp, Warning, TEXT("Name : %x"), *GetName());

	UParticleSystemComponent* Particle = nullptr;
	// 획득 이펙트
	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupParticle, GetActorLocation(), GetActorRotation(), true);
	}
	// 획득 사운드
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}

	// IsValid() checks, if an object is non-null and not marked ready for being garbage collected during the next garbage collection cycle.
	if (IsValid(Particle))
	{
		FTimerHandle DestroyParticleTimerHandle;

		TWeakObjectPtr<UPrimitiveComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				// 일반 포인터는 삭제가 되어도 주소 값은 여전히 같은 주소를 가리킴, 대신 안에 내용물은 없어짐
				// 그래서 C++에서도 많이 본 것처럼 쓰레기 값이 들어가 있는(삭제된) 주소에 있는 걸 읽을라고하니까 에러 발생
				// ▶ WeakPtr을 사용해서 해당 객체에 접근을 할 수 있나 없나(삭제가 되었나 안되었나)를 확인하면 문제 없이 동작함
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.0f,
			false
		);
	}
}

FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::DestroyItem()
{
	Destroy();
}