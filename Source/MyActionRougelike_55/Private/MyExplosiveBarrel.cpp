// @ajinkyax

#include "MyExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AMyExplosiveBarrel::AMyExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->SetAutoActivate(false);
	RadialForceComp->Radius = 600.f;
	RadialForceComp->ImpulseStrength = 1200.f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);

	ExplosionImpulse = 1000.f;
	ExplosionRadius = 750.f;
	ExplosionStrength = 2000.f;
}

void AMyExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	MeshComp->OnComponentHit.AddDynamic(this, &AMyExplosiveBarrel::OnHit);
}

void AMyExplosiveBarrel::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void AMyExplosiveBarrel::Explode()
{
	// Apply upward impulse to the barrel
	MeshComp->AddImpulse(FVector(0, 0, ExplosionImpulse), NAME_None, true);

	// Fire radial force
	RadialForceComp->FireImpulse();

	// Spawn explosion effect at the bottom of the barrel
	if (BoomFX)
	{
		const FVector ExplosionLocation = GetActorLocation() - FVector(0, 0, MeshComp->Bounds.BoxExtent.Z-50.f);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BoomFX, ExplosionLocation);
	}
}