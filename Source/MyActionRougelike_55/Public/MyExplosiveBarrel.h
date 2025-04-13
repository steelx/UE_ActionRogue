// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class URadialForceComponent;

UCLASS()
class MYACTIONROUGELIKE_55_API AMyExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URadialForceComponent* RadialForceComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* BoomFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UMaterialInstanceDynamic* DynamicMaterial;

	/* Barrel use previously */
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bUse;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/* Impulse strength when hit */
	UPROPERTY(EditDefaultsOnly, Category = "Physics")
	float ExplosionImpulse;

	/* Radius of the explosion force */
	UPROPERTY(EditDefaultsOnly, Category = "Physics")
	float ExplosionRadius;

	/* Strength of the explosion force */
	UPROPERTY(EditDefaultsOnly, Category = "Physics")
	float ExplosionStrength;

};
