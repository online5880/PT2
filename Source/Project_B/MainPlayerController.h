// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_B_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void OnPossess(APawn* InPawn) override;

	/************************ UI ************************/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="UI")
	TSubclassOf<class UMainWidget> Main_Widget_Class;

	class UMainWidget* Main_Widget;
};
