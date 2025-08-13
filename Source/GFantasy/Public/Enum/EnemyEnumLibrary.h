// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyEnumLibrary.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	/**
	 * <summary>
	 * 일반 Idle 상태
	 * </summary>
	 */
	Passive UMETA(DisplayName = "Passive"),

	/**
	 * <summary>
	 * 순찰 상태
	 * </summary>
	 */
	Patrol UMETA(DisplayName = "Patrol"),

	/**
	 * <summary>
	 * 위치를 경계 상태 하는 상태
	 * </summary>
	 */
	InvestigationLocation UMETA(DisplayName = "InvestigationLocation"),

	/**
	 * <summary>
	 * 액터를 경계 하는 상태
	 * </summary>
	 */
	InvestigationActor UMETA(DisplayName = "InvestigationActor"),

	/**
	 * <summary>
	 * 공격 상태
	 * </summary>
	 */
	Attack UMETA(DisplayName = "Attack"),

	/**
	 * <summary>
	 * 특수 패턴 진행 상태
	 * </summary>
	 */
	Pattern UMETA(DisplayName = "Pattern"),

	/**
	 * <summary>
	 * 그로기 상태
	 * </summary>
	 */
	Groggy UMETA(DisplayName = "Groggy"),

	/**
	 * <summary>
	 * 그로기 상태
	 * </summary>
	 */
	Hit UMETA(DisplayName = "Hit")
};

UENUM(BlueprintType)
enum class EEnemyRotateMode : uint8
{
	/**
	 * <summary>
	 * 포커스 모드
	 * </summary>
	 */
	FocusMode UMETA(DisplayName = "FocusMode"),

	/**
	 * <summary>
	 * 부드럽게 회전하는 상태
	 * </summary>
	 */
	RoundMode UMETA(DisplayName = "RoundMode"),
};


class GFANTASY_API EnemyEnumLibrary
{
public:
	EnemyEnumLibrary();
	~EnemyEnumLibrary();
};
