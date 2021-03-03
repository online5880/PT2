// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "AITypes.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Components/SceneComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "MainCameraShake.h"
#include "Sound/SoundCue.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Enemy_blood = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Blood"));
	Enemy_blood->SetupAttachment(GetMesh());

	Sword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	Sword->SetupAttachment(GetMesh(),FName("Weapon_Socket"));
	Sword->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sword->SetCollisionResponseToAllChannels(ECR_Ignore);

	Sword_Collision =CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sword_Collision"));
	Sword_Collision->SetupAttachment(GetMesh(),FName("Weapon_Socket"));
	Sword_Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Combat_Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Combat_Collision"));
	Combat_Collision->SetupAttachment(RootComponent);
	Combat_Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Combat_Collision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	Agro_Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Agro_Collision"));
	Agro_Collision->SetupAttachment(RootComponent);
	Agro_Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Agro_Collision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	bDie= false;
	bAttacking = false;
	bStrongAttack = false;
	bKnockBack = false;
	bTarget =false;
	bCombatCollision_Target = false;
	bRage = false;

	Attack_Montage_Num = 0;
	
	ResetTime = 5.5f;
	fKnockBack_Reset = 10.f;

	MaxHealth = 200.f;
	Health = 40.f;
	Attack_Damage = 20.f;

	static ConstructorHelpers::FObjectFinder<USoundWave>
            Shield_React(TEXT("SoundWave'/Game/Sound/Shield_Counter.Shield_Counter'"));
	if(Shield_React.Succeeded())
	{
		KnockBack_React_Sound = Shield_React.Object;
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Sword_Collision->OnComponentBeginOverlap.AddDynamic(this,&AEnemy::SwordOnOverlapBegin);
	Sword_Collision->OnComponentEndOverlap.AddDynamic(this,&AEnemy::SwordOnOverlapEnd);

	Combat_Collision->OnComponentBeginOverlap.AddDynamic(this,&AEnemy::CombatOnOverlapBegin);
	Combat_Collision->OnComponentEndOverlap.AddDynamic(this,&AEnemy::CombatOverlapEnd);

	Agro_Collision->OnComponentBeginOverlap.AddDynamic(this,&AEnemy::AgroOnOverlapBegin);
	Agro_Collision->OnComponentEndOverlap.AddDynamic(this,&AEnemy::AgroOverlapEnd);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	AIController = Cast<AAIController>(GetController());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bAttacking)
	{
		ToTarget();
	}
	if(bStrongAttack)
	{
		Attack_Damage = 40.f;
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartMatineeCameraShake(ShakeClass, 1.0f, ECameraShakePlaySpace::CameraLocal, FRotator(0.f));
	}
	else
	{
		Attack_Damage = 20.f;
	}
	if(Health <= 30.f)
	{
		bRage = true;
	}
	else{ bRage = false; }
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
/********************************** AI ********************************/
void AEnemy::MoveToTarget(class AMain* Target)
{
	if(AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(250.f);

		FNavPathSharedPtr NavPath;

		//Interp_To();
		AIController->MoveTo(MoveRequest,&NavPath);
	}
}
/********************************** 전투 ********************************/
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp,Warning,TEXT("Enemy TakeDamage : %f, Health : %f"),Damage,Health);

	Health-=Damage;

	FRotator b = FRotator(FMath::RandRange(90.f,-90.f));

	Enemy_blood->Activate(true);
	Enemy_blood->SetWorldRotation(b);
	if(Health<=0)
	{
		Die();
		bDie = true;
		bAttacking =false;
	}
	return Damage;
}

void AEnemy::Attack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && !bKnockBack && bTarget)
	{
		float Play_Rate;
		const uint8 Attack_Montage = FMath::RandRange(0,2);

		if(bRage)
		{
			Play_Rate =  FMath::RandRange(1.f,1.5f);
		}
		else
		{
			Play_Rate =  FMath::RandRange(0.7f,0.8f);
		}
		
		if(Attack_Montage == 0)
		{
			AnimInstance->Montage_Play(Attack_Montage_1,Play_Rate);
			bAttacking = true;
		}
		if(Attack_Montage == 1)
		{
			AnimInstance->Montage_Play(Attack_Montage_2,Play_Rate);
			bAttacking = true;
		}
		if(Attack_Montage == 2)
		{
			AnimInstance->Montage_Play(Attack_Montage_3,Play_Rate);
			bAttacking = true;
		}
		GetWorldTimerManager().SetTimer(AttackResetTimer,this,&AEnemy::AttackEnd,ResetTime,true);
	}
}

void AEnemy::SwordOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;

	if(OtherActor)
	{	
		Main = Cast<AMain>(OtherActor);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(Main)
		{
			UE_LOG(LogTemp,Warning,TEXT("Enemy Attack : %s"),*OtherActor->GetName());
			UGameplayStatics::ApplyDamage(Main,Attack_Damage,nullptr,GetOwner(),nullptr);
		}
		if(!bKnockBack && Main->bGuardAttack)
		{
			//AnimInstance->Montage_Stop(1.f,Attack_Montage);
			KnockBack();
			UE_LOG(LogTemp,Warning,TEXT("Enemy = KnockBack"))

			UGameplayStatics::PlaySound2D(this,KnockBack_React_Sound);
			if(bAttacking)
			{
				Attack();
			}
		}
	}
}

void AEnemy::SwordOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;
}

// 이동 중 방향 전환 보간
void AEnemy::Interp_To()
{
	FVector Enemy_Location = GetMesh()->GetComponentLocation();
	
	float Delta = (GetWorld()->DeltaTimeSeconds)*5.f;
	UE_LOG(LogTemp,Warning,TEXT("Interp_To"));

	FRotator Enemy_R = GetMesh()->GetComponentRotation();
	
	FVector Target_Location = Main->GetActorLocation();
	FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Enemy_Location,Target_Location);
	FRotator Enemy_Rotation = FRotator(0.f,Rotator.Yaw,0.f);
	//GetCapsuleComponent()->SetWorldRotation(Enemy_Rotation);
	UKismetMathLibrary::RInterpTo(Enemy_R,Enemy_Rotation,Delta,20.f);
}

// 공격 중 캐릭터를 향해 방향 전환 보간
void AEnemy::ToTarget() 
{
	FVector Enemy_Location = GetActorLocation();
	FVector Target_Location = Main->GetActorLocation();
	float Delta = (GetWorld()->DeltaTimeSeconds);
	float It_Speed = 1.f;
	
	FRotator Enemy_R = GetActorRotation();
	FRotator Target_R = Main->GetMesh()->GetComponentRotation();
	
	if(bRage)
	{
		It_Speed = 2.f;
	}

	FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Enemy_Location,Target_Location);
	FRotator Enemy_Rotation = FRotator(0.f,Rotator.Yaw,0.f);
	FRotator Test = UKismetMathLibrary::RInterpTo(Enemy_R,Enemy_Rotation,Delta,It_Speed);
	SetActorRotation(Test);
}

void AEnemy::Sword_Collision_On()
{
	Sword_Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::Sword_Collision_Off()
{
	Sword_Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;

	if(OtherActor)
	{	
		Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			bAttacking = true;
			bCombatCollision_Target = true;
			
			if(bAttacking && !bDie && !bKnockBack
				&& !AnimInstance->Montage_IsPlaying(Attack_Montage_1)
				&& !AnimInstance->Montage_IsPlaying(Attack_Montage_2)
				&& !AnimInstance->Montage_IsPlaying(Attack_Montage_3))
			{
				Attack();
			}
		}
	}
}

void AEnemy::CombatOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;

	if(OtherActor)
	{	
		Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			bCombatCollision_Target = false;;
			
			if(!bDie && bTarget)
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if(AnimInstance)
				{
					//AnimInstance->Montage_Stop(1.f,Attack_Montage);
					MoveToTarget(Main);
				}
			}
		}
	}
}

void AEnemy::AgroOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor==this) return;
	if(OtherActor==GetOwner()) return;

	if(OtherActor)
	{	
		Main = Cast<AMain>(OtherActor);

		bTarget = true;
		if(Main && bTarget)
		{
			MoveToTarget(Main);
			UE_LOG(LogTemp,Warning,TEXT("Target In"));
			
		}
	}
}

void AEnemy::AgroOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp,Warning,TEXT("Target Out"));
	bTarget = false;
	AIController->StopMovement();
}

void AEnemy::AttackEnd()
{
	ResetTime = 5.f;
	bAttacking = false;
	if(!bDie && bTarget)
	{
		MoveToTarget(Main);

		const uint8 Attack_Montage = FMath::RandRange(0,2);
		float Play_Rate;

		if(bRage)
		{
			Play_Rate =  FMath::RandRange(1.f,1.5f);
		}
		else
		{
			Play_Rate =  FMath::RandRange(0.7f,0.8f);
		}
		
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance && bCombatCollision_Target)
		{
			if(Attack_Montage == 0)
			{
				AnimInstance->Montage_Play(Attack_Montage_1,Play_Rate);
				bAttacking = true;
			}
			if(Attack_Montage == 1)
			{
				AnimInstance->Montage_Play(Attack_Montage_2,Play_Rate);
				bAttacking = true;

			}
			if(Attack_Montage == 2)
			{
				AnimInstance->Montage_Play(Attack_Montage_3,Play_Rate);
				bAttacking = true;
			}
		}
	}
}

void AEnemy::KnockBack()
{
	bKnockBack = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && bTarget)
	{
		AnimInstance->Montage_Play(KnockBack_Montage);
		MoveToTarget(Main);
		Main->SetGuardAttack(false);
		ResetTime = 1.5f;
		GetWorldTimerManager().SetTimer(KnockBack_ResetTimer,this,&AEnemy::KnockBack_End,fKnockBack_Reset,false);
	}
}

void AEnemy::KnockBack_End()
{
	bKnockBack = false;
	//Combat_Collision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

/********************************** 죽음 ********************************/
void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance)
	{
		AnimInstance->Montage_Play(Die_Montage);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sword_Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Combat_Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnemy::Die_End()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance)
	{
		AnimInstance->Montage_Stop(1.f,Die_Montage);
	}
}
