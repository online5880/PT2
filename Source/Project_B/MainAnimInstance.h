// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_B_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UMainAnimInstance();

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Movement")
	float MovementSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Movement")
	bool bIsAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn")
	class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main")
	class AMain* Main;
	
	
};
