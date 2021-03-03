// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"

#include "EnemyWidget.h"

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(Enemy_Widget_Class)
	{
		Enemy_Widget = CreateWidget<UEnemyWidget>(this,Enemy_Widget_Class);
		if(Enemy_Widget)
		{
			Enemy_Widget->AddToViewport();
		}
	}
}
