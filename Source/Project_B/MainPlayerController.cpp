// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "MainWidget.h"

void AMainPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(Main_Widget_Class)
	{
		Main_Widget = CreateWidget<UMainWidget>(this,Main_Widget_Class);
		if(Main_Widget)
		{
			Main_Widget->AddToViewport();
		}
	}
}
