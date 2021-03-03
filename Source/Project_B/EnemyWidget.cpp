// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidget.h"

void UEnemyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HealthBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar")));
}
