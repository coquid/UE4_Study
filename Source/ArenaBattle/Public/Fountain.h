// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Fountain.generated.h" // UE Object Ŭ������ ���� �κ�


UCLASS() // UE Object Ŭ������ ���� �κ�
class ARENABATTLE_API AFountain : public AActor // UE Object Ŭ������ ���� �κ�
{
	GENERATED_BODY() // UE Object Ŭ������ ���� �κ�
	
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

	// UPROPERTY�� �ִ� ���� = UE�� GC�� ������ ��ü�� �˼� �ְ� �Ϸ���.
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
