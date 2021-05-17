// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Fountain.generated.h" // UE Object 클래스를 위한 부분


UCLASS() // UE Object 클래스를 위한 부분
class ARENABATTLE_API AFountain : public AActor // UE Object 클래스를 위한 부분
{
	GENERATED_BODY() // UE Object 클래스를 위한 부분
	
public:	
	// Sets default values for this actor's properties
	AFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// UPROPERTY를 넣는 이유 = UE의 GC가 적절히 객체를 알수 있게 하려고.
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Body;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Water;

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* Light;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* Splash;

	UPROPERTY(EditAnywhere, Category="ID")
	int32 ID;

	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* Movement;

private:
	UPROPERTY(EditAnywhere, Category="Stat", meta=(AllowPrivateAccess = true))
	float RotateSpeed;
};
