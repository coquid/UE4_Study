// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/SaveGame.h"
#include "ABSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UABSaveGame();

	UPROPERTY()
	int32 Level = 1;

	UPROPERTY()
	int32 Exp = 0;

	UPROPERTY()
	FString PlayerName = TEXT("Guest");

	UPROPERTY()
	int32 HighScore = 0;
};
