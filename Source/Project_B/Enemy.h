// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class PROJECT_B_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	/************************ 무기 ************************/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Weapon")
	class UStaticMeshComponent* Sword;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Weapon")
	class UCapsuleComponent* Sword_Collision;
	/************************ AI *************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UFUNCTION(BlueprintCallable)
    void MoveToTarget(class AMain* Main);
	/************************ 전투 ************************/
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const & DamageEvent,class AController * EventInstigator,AActor * DamageCauser) override;

	UFUNCTION()
    virtual void SwordOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void SwordOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class AMain* Main;

	UFUNCTION(BlueprintCallable)
	void Interp_To();

	UFUNCTION(BlueprintCallable)
    void ToTarget();

	UFUNCTION(BlueprintCallable)
    void Sword_Collision_On();

	UFUNCTION(BlueprintCallable)
    void Sword_Collision_Off();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* Attack_Montage_1;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* Attack_Montage_2;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* Attack_Montage_3;

	uint32 Attack_Montage_Num;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UNiagaraComponent* Enemy_blood;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Weapon")
	class USphereComponent* Combat_Collision;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Weapon")
	class USphereComponent* Agro_Collision;

	UFUNCTION()
    virtual void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void CombatOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
    virtual void AgroOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void AgroOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bAttacking;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bCombatCollision_Target;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bTarget;

	UFUNCTION()
	void Attack();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	float Attack_Damage;

	float Attack_Damage1() const
	{
		return Attack_Damage;
	}

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bStrongAttack;

	bool StrongAttack() const
	{
		return bStrongAttack;
	}

	void SetStrongAttack(bool boolStrongAttack)
	{
		this->bStrongAttack = boolStrongAttack;
	}

	UFUNCTION(BlueprintCallable)
	void Strong_Attack_On() {SetStrongAttack(true);}

	UFUNCTION(BlueprintCallable)
	void Strong_Attack_Off() {SetStrongAttack(false);}

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bRage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<class UMainCameraShake> ShakeClass;

	FTimerHandle AttackResetTimer;

	float ResetTime;

	UFUNCTION(BlueprintCallable)
    void AttackEnd();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* KnockBack_Montage;

	UFUNCTION()
	void KnockBack();

	UFUNCTION(BlueprintCallable)
	void KnockBack_End();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bKnockBack;

	void SetKnockBack(bool KnockBack)
	{
		this->bKnockBack = KnockBack;
	}

	FTimerHandle KnockBack_ResetTimer;

	float fKnockBack_Reset;

	class USoundWave* KnockBack_React_Sound;


	/************************ 스탯 ************************/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	float MaxHealth;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	float Health;

	/************************ 죽음 ************************/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* Die_Montage;

	void Die();

	UFUNCTION(BlueprintCallable)
	void Die_End();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bDie;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
