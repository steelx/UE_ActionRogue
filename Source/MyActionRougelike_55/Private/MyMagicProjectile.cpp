// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMagicProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AMyMagicProjectile::AMyMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	// Collisions
	/*
	 * Either use collision settings 
	SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	 * Or use Custom profile
	SphereComp->SetCollisionProfileName(TEXT("Projectile"));
	 */
	// making changes to collision needs full re-run project
	SphereComp->SetCollisionProfileName(TEXT("Projectile"));

	SphereComp->SetNotifyRigidBodyCollision(true);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AMyMagicProjectile::OnActorOverlap);
	SphereComp->OnComponentHit.AddDynamic(this, &AMyMagicProjectile::OnHit);

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->InitialSpeed = 1000.f;
	MovementComp->MaxSpeed = 1600.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0.2f;
	MovementComp->bShouldBounce = false;
	MovementComp->OnProjectileStop.AddDynamic(this, &AMyMagicProjectile::OnProjectileStop);
}

// Called when the game starts or when spawned
void AMyMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyMagicProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != GetInstigator())
	{
		// UE_LOG(LogTemp, Warning, TEXT("Hit with Actor: %s"), *OtherActor->GetName());
		// if (OtherComp)
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *OtherComp->GetName());
		// }
		Explode();
	}
}

void AMyMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor != GetInstigator())
	{
		// UE_LOG(LogTemp, Warning, TEXT("Overlapped with Actor: %s"), *OtherActor->GetName());
		// if (OtherComp)
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Overlapped Component: %s"), *OtherComp->GetName());
		// }
		Explode();
	}

}

void AMyMagicProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnProjectileStop Triggered! Stopping against: %s"), ImpactResult.GetActor() ? *ImpactResult.GetActor()->GetName() : TEXT("None"));
	Explode();
}

void AMyMagicProjectile::Explode()
{
	// Spawn explosion effect if set
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation(), GetActorRotation());
	}
	
	// Destroy the projectile after the next tick
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		Destroy();
	});
}

