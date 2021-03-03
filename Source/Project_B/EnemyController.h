// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_B_API AEnemyController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void OnPossess(APawn* InPawn) override;

	/************************ UI ************************/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="UI")
	TSubclassOf<class UEnemyWidget> Enemy_Widget_Class;

	class UEnemyWidget* Enemy_Widget;
	
};
