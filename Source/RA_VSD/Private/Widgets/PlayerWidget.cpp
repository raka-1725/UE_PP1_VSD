// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerWidget.h"

#include "Components/Overlay.h"

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InteractOverlay->SetVisibility(ESlateVisibility::Hidden);
}
void UPlayerWidget::BeginPlay()
{
	InteractOverlay->SetVisibility(ESlateVisibility::Hidden);
}
void UPlayerWidget::ShowCanInteract()
{
	InteractOverlay->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerWidget::HideCanInteract()
{
	InteractOverlay->SetVisibility(ESlateVisibility::Hidden);
}
