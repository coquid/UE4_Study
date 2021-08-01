// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeComponent() override;
	
public:
	void SetNewLevel(int32 NewLevel);
	void SetDamage(float NewDamage);
	float GetAttack();

	FOnHPIsZeroDelegate OnHPIsZero;

private:
	struct FABCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category=Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	// CurrentHP�� ������ ������ �� �Ź� ����Ǳ� ������, ���� �����ϴ°� �ǹ� ��� Serialize �� �ʿ䰡 ����.
	// ���� Transient �÷��׸� ���ؼ� ����ȭ�� ���ܽ�Ű��.
	UPROPERTY(Transient, VisibleInstanceOnly ,Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;
};