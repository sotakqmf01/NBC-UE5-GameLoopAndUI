// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class BASICCHARACTER_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	// IMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|IMC")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|IMC")
	UInputMappingContext* TabInputMappingContext;

	// IA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|IA")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|IA")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|IA")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|IA")
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|IA")
	UInputAction* TapKeyAction;

	// UI 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "HUD")
	UUserWidget* HUDWidgetInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Menu")
	UUserWidget* MainMenuWidgetInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> TabMenuWidgetClass;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Menu")
	UUserWidget* TabMenuWidgetInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> GameOverWidgetClass;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Menu")
	UUserWidget* GameOverWidgetInstance;

	UInputMappingContext* CurrIMC;

	bool bIsTabMenuOff;
	bool bInGame;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	//UFUNCTION()
	//void ChangeIMC(const FInputActionValue& value);

public:
	UFUNCTION(BlueprintPure, Category = "HUD")
	UUserWidget* GetHUDWidget() const;
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainMenu();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowGameOverMenu(bool bIsClear);
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartMainMenu();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void TabMenu(const FInputActionValue& value);

	void ExitGame();	// 게임 종료
};
