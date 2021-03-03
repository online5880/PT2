// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



#include "MainWidget.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UCLASS()
class PROJECT_B_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	/************************ 카메라 ***********************/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Camera")
	class UCameraComponent* Camera;
	/************************ UI ************************/
	class UMainWidget* MainWidget;
	/************************ 스탯 ************************/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Stat")
	float MaxHealth;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Stat")
	float Health;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Stat")
	float MaxStamina;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Stat")
	float Stamina;
	/************************ 무기 ************************/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Weapon")
	class UStaticMeshComponent* Sword;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Weapon")
	class UStaticMeshComponent* Shield;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Weapon")
	class UCapsuleComponent* Sword_Collision;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Weapon")
	class UBoxComponent* Shield_Collision;
	/************************ 이동 ************************/
	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void StartJump();

	UFUNCTION(BlueprintCallable)
	void StopJump();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Movement")
	bool bJump;
	
	UFUNCTION()
	void StartSprint();

	UFUNCTION()
	void StopSprint();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Movement")
	bool bSprint;
	/************************ 구르기 ************************/
	UFUNCTION()
    void StartRoll();

	UFUNCTION(BlueprintCallable)
    void StopRoll();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bRoll;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* Roll_Montage;
	/************************ 방어 ************************/
	UFUNCTION()
    void StartGuard();

	UFUNCTION()
    void StopGuard();

	UFUNCTION()
	void GuardAttack();

	UFUNCTION(BlueprintCallable)
    void GuardAttack_End() { bGuardAttack = false;}

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bGuardAttack;

	void SetGuardAttack(bool GuardAttack)
	{
		this->bGuardAttack = GuardAttack;
	}

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bGuard;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* Guard_Montage;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* GuardAttack_Montage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	class USoundCue* Guard_Sound;

	UFUNCTION()
    virtual void ShieldOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void ShieldOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/************************ 전투 ************************/

	class AEnemy* Enemy;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	class UCapsuleComponent* Combat_Capsule;

	UFUNCTION()
    virtual void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bTarget;

	uint32 ComboCount;
	
	UFUNCTION()
	void LMBDown();

	UFUNCTION()
	void LMBUp();
	
	UFUNCTION()
    void StartAttack();

	UFUNCTION(BlueprintCallable)
    void StopAttack();

	UFUNCTION()
	void ComboReset();

	UFUNCTION(BlueprintCallable)
	void Sword_Collision_On();

	UFUNCTION(BlueprintCallable)
    void Sword_Collision_Off();

	UFUNCTION(BlueprintCallable)
    void ToTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<class AEnemy> GetEnemy;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bAttack;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bLMBDown;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* Attack_Montage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UNiagaraComponent* blood;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<class UMainCameraShake> ShakeClass;

	FTimerHandle ComboResetTimer;

	float ResetTime;

	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const & DamageEvent,class AController * EventInstigator,AActor * DamageCauser) override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bTakeDamage;

	UFUNCTION(BlueprintCallable)
	void CheckDamage();
	
	UFUNCTION()
	virtual void SwordOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void SwordOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* React_Montage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* KnockDown_Montage;

	bool bKnockDown;
	/************************ 죽음 ************************/
	UFUNCTION()
	void Die();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bDie;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage* Die_Montage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
