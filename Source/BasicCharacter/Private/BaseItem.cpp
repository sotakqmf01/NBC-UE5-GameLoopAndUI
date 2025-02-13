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

	// �̺�Ʈ ���ε�
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverlap);
}

void ABaseItem::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// tag�� �г��� ���� ��
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
	// ȹ�� ����Ʈ
	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupParticle, GetActorLocation(), GetActorRotation(), true);
	}
	// ȹ�� ����
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
				// �Ϲ� �����ʹ� ������ �Ǿ �ּ� ���� ������ ���� �ּҸ� ����Ŵ, ��� �ȿ� ���빰�� ������
				// �׷��� C++������ ���� �� ��ó�� ������ ���� �� �ִ�(������) �ּҿ� �ִ� �� ��������ϴϱ� ���� �߻�
				// �� WeakPtr�� ����ؼ� �ش� ��ü�� ������ �� �� �ֳ� ����(������ �Ǿ��� �ȵǾ���)�� Ȯ���ϸ� ���� ���� ������
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