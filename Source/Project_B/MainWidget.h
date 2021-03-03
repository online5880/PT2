// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Main.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"



#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_B_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	
	virtual void NativeConstruct() override;


	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* StaminaBar;
	
};
