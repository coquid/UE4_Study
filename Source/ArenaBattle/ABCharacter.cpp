// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "ABWeapon.h"

// Sets default values
AABCharacter::AABCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}
	else
	{
		ABLOG(Warning, TEXT("Fail to set skeletal mesh!"));
	}

	// Blueprint version animation 세팅
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBluePrint.WarriorAnimBluePrint.WarriorAnimBluePrint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	SetControlMode(EControlMode::DIABLO);
	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	IsAttacking = false;
	MaxCombo = 4;
	AttackEndComboState();

	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	// 프리셋 지정.
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));


}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//FName WeaponSoket(TEXT("hand_rSocket"));
	//if (GetMesh()->DoesSocketExist(WeaponSoket))
	//{
	//	auto CurWeapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	//	if (CurWeapon != nullptr)
	//	{
	//		CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSoket);
	//	}
	//}
}

void AABCharacter::SetControlMode(AABCharacter::EControlMode ControlMode)
{
	CurrentControlMode = ControlMode;

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		// SpringArm->TargetArmLength = 450.f;
		// SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		ArmLengthTo = 450.0f;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;

	case EControlMode::DIABLO:
		// SpringArm->TargetArmLength = 800.0f;
		// SpringArm->SetRelativeRotation(FRotator(-45.0f,0.0f,0.0f));
		ArmLengthTo = 800.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;

	default:
		break;
	}
}

void AABCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(ABAnim != nullptr);

	ABAnim->OnMontageEnded.AddDynamic(this,&AABCharacter::OnAttackMontageEnded);

	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;
		if (IsComboInputOn)
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontage(CurrentCombo);
		}
		});

	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);
}

// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// lenght interpolation
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::DIABLO:
		FRotator interpRotation = FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed);
		SpringArm->SetRelativeRotation(interpRotation);
		break;
	}


	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		break;
	case AABCharacter::EControlMode::DIABLO:
		if (DirectionToMove.SizeSquared() > 0.0f)
		{

			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
		break;
	default:
		break;
	}
}

float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took damage : %f"), *GetName(), FinalDamage);

	if (FinalDamage > 0.0f)
	{
		ABAnim->SetDeadAnim(); // 죽는 anim 재생.
		SetActorEnableCollision(false); // 죽으면 collision 끔.
	}
	return FinalDamage;
}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack);
}

bool AABCharacter::CanSetWeapon()
{
	return (CurrentWeapon == nullptr);
}

void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(NewWeapon != nullptr && CurrentWeapon == nullptr);
	FName WeaponSocket = TEXT("hand_rSocket");
	if (NewWeapon != nullptr)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void AABCharacter::UpDown(float NewAxisAvlue)
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		// SpringArm의 회전 값은 Control 회전값과 동일함.
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisAvlue); // RootComponent의 ForwardVector를 가져옴.
		break;
	case AABCharacter::EControlMode::DIABLO:
		DirectionToMove.X = NewAxisAvlue;
		break;
	default:
		break;
	}
	
}

void AABCharacter::LeftRight(float NewAxisAvlue)
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisAvlue); // RootComponent의 RightVector를 가져옴.
		break;
	case AABCharacter::EControlMode::DIABLO:
		DirectionToMove.Y = NewAxisAvlue;
		break;
	default:
		break;
	}

}

void AABCharacter::LookUp(float NewAxisAvlue)
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddControllerPitchInput(NewAxisAvlue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		break;
	default:
		break;
	}
}

void AABCharacter::Turn(float NewAxisAvlue)
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddControllerYawInput(NewAxisAvlue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		break;
	default:
		break;
	}

}

void AABCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
	{
		SpringArm->SetRelativeRotation(GetController()->GetControlRotation());
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIABLO);
		break;
	}
	case AABCharacter::EControlMode::DIABLO:
		GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
		SetControlMode(EControlMode::GTA);
		break;
	default:
		break;
	}
}

void AABCharacter::Attack()
{
	if (IsAttacking)
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontage(CurrentCombo);
		IsAttacking = true;
	}
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}

void AABCharacter::AttackStartComboState()
{
	IsComboInputOn = false;
	CanNextCombo = true;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AABCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AABCharacter::AttackCheck()
{
	FHitResult hitResult;
	FCollisionQueryParams Params(NAME_None, false, this); // this 를 넣어서 ignore 옵션을 지정. 자기 자신은 체크되면 안되서.
	bool bResult = GetWorld()->SweepSingleByChannel(
		hitResult
		, GetActorLocation()
		, GetActorLocation() + GetActorForwardVector() * AttackRange
		, FQuat::Identity
		, ECollisionChannel::ECC_GameTraceChannel2
		, FCollisionShape::MakeSphere(AttackRadius), Params);

#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebufLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld()
		, Center
		, HalfHeight
		, AttackRadius
		, CapsuleRot
		, DrawColor
		, false
		, DebufLifeTime);
#endif


	if (bResult)
	{
		ABLOG(Warning, TEXT("Hit actor name = %s"), *hitResult.Actor->GetName());

		FDamageEvent DamageEvent;
		hitResult.Actor->TakeDamage(50.0f, DamageEvent, GetController(), this);
	}


}
