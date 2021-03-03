// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "MainAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "MainCameraShake.h"
#include "NiagaraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Blueprint/UserWidget.h"


// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 350.f;
	SpringArm->bUsePawnControlRotation = true;	

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	Sword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	Sword->SetupAttachment(GetMesh(),FName("S_Sword"));

	Sword_Collision =CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sword_Collision"));
	Sword_Collision->SetupAttachment(GetMesh(),FName("S_Sword"));
	Sword_Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Shield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield"));
	Shield->SetupAttachment(GetMesh(),FName("S_Shield"));

	Shield_Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Shield_Collision"));
	Shield_Collision->SetupAttachment(GetMesh(),FName("S_Shield"));
	Shield_Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Shield_Collision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Combat_Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Combat_Capsule"));
	Combat_Capsule->SetupAttachment(GetRootComponent());
	
	blood = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Blood"));
	blood->SetupAttachment(Sword);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 350.f;

	bJump = false;
	bSprint = false;
	bRoll = false;
	bGuard =false;
	bGuardAttack = false;
	bAttack = false;
	bLMBDown = false;
	bTakeDamage =false;
	bKnockDown = false;
	bDie = false;
	bTarget= false;

	ComboCount = 0;
	ResetTime = 3.f;

	MaxHealth = 100.f;
	Health = 21.f;

	MaxStamina = 200.f;
	Stamina = 100.f;
}
// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	Sword_Collision->OnComponentBeginOverlap.AddDynamic(this,&AMain::SwordOnOverlapBegin);
	Sword_Collision->OnComponentEndOverlap.AddDynamic(this,&AMain::SwordOnOverlapEnd);

	Shield_Collision->OnComponentBeginOverlap.AddDynamic(this,&AMain::ShieldOnOverlapBegin);
	Shield_Collision->OnComponentEndOverlap.AddDynamic(this,&AMain::ShieldOnOverlapEnd);

	Combat_Capsule->OnComponentBeginOverlap.AddDynamic(this,&AMain::CombatOnOverlapBegin);
	Combat_Capsule->OnComponentEndOverlap.AddDynamic(this,&AMain::CombatOnOverlapEnd);
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(!GetMovementComponent()->IsFalling())
	{
		bJump =false;
	}
	if (Stamina >= MaxStamina)
	{
		Stamina = MaxStamina;
	}
	if(Stamina<=0.f)
	{
		Stamina= 0;
		GetCharacterMovement()->MaxWalkSpeed = 350.f;
	}
	
	if (bSprint)
	{
		Stamina -= DeltaTime * 20;
	}
	if(!(bRoll && bGuardAttack && bAttack && bDie && bJump && bSprint))
	{
		Stamina += DeltaTime * 15;
		if (Stamina >= MaxStamina)
		{
			Stamina = MaxStamina;
		}
	}
	if(bTarget && bAttack)
	{
		ToTarget();
	}
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward",this,&AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&AMain::MoveRight);
	PlayerInputComponent->BindAxis("Turn",this,&ACharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",this,&ACharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&AMain::StartJump);
	//PlayerInputComponent->BindAction("Jump",IE_Released,this,&AMain::StopJump);
	PlayerInputComponent->BindAction("Sprint",IE_Pressed,this,&AMain::StartSprint);
	PlayerInputComponent->BindAction("Sprint",IE_Released,this,&AMain::StopSprint);
	PlayerInputComponent->BindAction("Roll",IE_Pressed,this,&AMain::StartRoll);
	PlayerInputComponent->BindAction("Guard",IE_Pressed,this,&AMain::StartGuard);
	PlayerInputComponent->BindAction("Guard",IE_Released,this,&AMain::StopGuard);
	PlayerInputComponent->BindAction("Attack",IE_Pressed,this,&AMain::LMBDown);
	PlayerInputComponent->BindAction("Attack",IE_Released,this,&AMain::LMBUp);

}
/********************************** 이동 ********************************/
void AMain::MoveForward(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	Direction.Z = 0.f;
	Direction.Normalize();
	AddMovementInput(Direction,Value);
	
}

void AMain::MoveRight(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	Direction.Z = 0.f;
	Direction.Normalize();
	AddMovementInput(Direction,Value);
}
/********************************** 점프 ********************************/
void AMain::StartJump()
{
	if(Stamina>=0.f)
	{
		bPressedJump = true;
		bJump =true;
		if(bJump)
		{
			Stamina -= 10.f;
		}
	}
}
void AMain::StopJump()
{
}
/********************************** 달리기 ********************************/
void AMain::StartSprint()
{
	if(Stamina>=0.f)
	{
		bSprint = true;
		if(bSprint)
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
		}
	}
}

void AMain::StopSprint()
{
	bSprint =false;
	if(!bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = 350.f;
	}
}
/********************************** 구르기 ********************************/
void AMain::StartRoll()
{	
	if(!bRoll && !bJump&&!bDie && Stamina>=0.f)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance)
		{
			AnimInstance->Montage_Play(Roll_Montage);
			bRoll=true;
			Stamina -= 30.f;
		}
	}
}

void AMain::StopRoll()
{
	bRoll=false;
}
/********************************** 방어 ********************************/
void AMain::StartGuard()
{
	bGuard=true;
	if(bGuard && !bRoll&&!bDie)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance)
		{
			AnimInstance->Montage_Play(Guard_Montage,1.5f);
			Shield_Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	
			UGameplayStatics::PlaySound2D(this,Guard_Sound);

			GuardAttack_End();
		}
	}
}

void AMain::StopGuard()
{
	bGuard = false;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f,Guard_Montage);
		Shield_Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMain::GuardAttack()
{
	bGuardAttack = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance)
	{
		AnimInstance->Montage_Play(GuardAttack_Montage);
		Stamina -= 15.f;
	}
}

void AMain::ShieldOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;

	if(OtherActor)
	{	
		Enemy = Cast<AEnemy>(OtherActor);
		if(Enemy)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if(AnimInstance)
			{
				if(bGuardAttack && bLMBDown)
				{
					UE_LOG(LogTemp,Warning,TEXT("Main -> Enemy = KnockBack"))

				}
				else
				{
					AnimInstance->Montage_Play(React_Montage);
					AnimInstance->Montage_JumpToSection(FName("Guard_React"),React_Montage);
					bTakeDamage = true;
					Stamina -= 30.f;
				}
			}
		}
	}
}

void AMain::ShieldOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;
}

void AMain::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;

	if(OtherActor)
	{
		Enemy = Cast<AEnemy>(OtherActor);
		if(Enemy)
		{
			bTarget= true;
		}
	}
}

void AMain::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;

	if(OtherActor)
	{
		Enemy = Cast<AEnemy>(OtherActor);
		if(Enemy)
		{
			bTarget= false;
		}
	}
}

void AMain::CheckDamage()
{
	bTakeDamage= false;
}
/********************************** 전투 ********************************/
void AMain::LMBDown()
{
	bLMBDown = true;
	if(!bAttack&&!bJump&&!bRoll)
	{
		StartAttack();

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance->Montage_IsPlaying(Attack_Montage))
		{
			ComboCount+=1;
		}
	}
}
void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::StartAttack()
{
	if(ComboCount>=3)
	{
		ComboCount=0;
	}

	GetWorldTimerManager().SetTimer(ComboResetTimer,this,&AMain::ComboReset,ResetTime,false);
	
	const char* Combo[]={"Attack_1","Attack_2","Attack_3"};
	
	if(!bJump&&!bRoll&&!bDie && Stamina>=0.f)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance && !bGuardAttack)
		{
			AnimInstance->Montage_Play(Attack_Montage);
			AnimInstance->Montage_JumpToSection(FName(Combo[ComboCount]),Attack_Montage);
			bAttack =true;
			Stamina -= 20.f;
		}
		if(bGuard && !bGuardAttack && Stamina>=0.f)
		{
			GuardAttack();
		}
	}
	UE_LOG(LogTemp,Warning,TEXT("Combo Count : %d"),ComboCount);
}

void AMain::StopAttack()
{
	bAttack =false;
}

void AMain::ComboReset()
{
	ComboCount = 0;
	UE_LOG(LogTemp,Warning,TEXT("Combo Reset"));
	UE_LOG(LogTemp,Warning,TEXT("Reset : Combo %d"),ComboCount);
}

void AMain::Sword_Collision_On()
{
	Sword_Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMain::Sword_Collision_Off()
{
	Sword_Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMain::ToTarget()
{	
	FVector Main_Location = GetActorLocation();
	FVector Enemy_Location = UGameplayStatics::GetActorOfClass(this,GetEnemy)->GetActorLocation();

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	FRotator Main_R = GetActorRotation();
	
	FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Main_Location,Enemy_Location);
	FRotator Enemy_Rotation = UKismetMathLibrary::MakeRotator(0.f,0.f,Rotator.Yaw);
	FRotator Set_Rotation = FMath::RInterpTo(Main_R,Enemy_Rotation,DeltaTime,15.f);
	SetActorRotation(Set_Rotation);
}

void AMain::SwordOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;

	if(OtherActor)
	{	
		Enemy = Cast<AEnemy>(OtherActor);
		if(Enemy)
		{
			if(Enemy->GetMesh())
			{

				blood->Activate(true);
				
				UGameplayStatics::ApplyDamage(Enemy,20.f,nullptr,GetOwner(),nullptr);

				UGameplayStatics::PlaySound2D(this,HitSound);
				
				GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartMatineeCameraShake(ShakeClass, 1.0f, ECameraShakePlaySpace::CameraLocal, FRotator(0.f));
			}
		}
	}
}

void AMain::SwordOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;
}


float AMain::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                        AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 구르기 또는 방어 중 데미지 0
	if(bRoll||bGuard)
	{
		Damage = 0;
		return Damage;
	}
	UE_LOG(LogTemp,Warning,TEXT("Main TakeDamage : %f"),Damage);

	Health-=Damage;
	
	if(Health<=0)
	{
		bDie = true;
		if(bDie)
		{
			Die();
			UE_LOG(LogTemp,Warning,TEXT("Die"));
			return Damage;
		}
	}
	// 랜덤으로 0, 1, 2 값을 줘서 피격 모션 출력
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartMatineeCameraShake(ShakeClass, 1.0f, ECameraShakePlaySpace::CameraLocal, FRotator(0.f));
	int React = FMath::RandRange(0,1);
	
	if(Damage >= 40) {React = 2;}

	if(AnimInstance)
	{
		
		if(React == 0)
		{
			AnimInstance->Montage_Play(React_Montage);
			AnimInstance->Montage_JumpToSection(FName("Hit_1"),React_Montage);
			bAttack = false;
		}
		if(React == 1)
		{
			AnimInstance->Montage_Play(React_Montage);
			AnimInstance->Montage_JumpToSection(FName("Hit_2"),React_Montage);
			bAttack = false;
		}
		if(React == 2)
		{
			AnimInstance->Montage_Play(React_Montage);
			AnimInstance->Montage_JumpToSection(FName("KnockBack"),React_Montage);
			bAttack = false;
		}
	}

	return Damage;
}
/************************ 죽음 ************************/
void AMain::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance)
	{
		AnimInstance->Montage_Play(Die_Montage);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetCharacterMovement()->DisableMovement();
		bAttack = false;
		bJump = false;
		bSprint = false;
		bRoll = false;
		bGuard =false;
		bTakeDamage =false;
	}
}