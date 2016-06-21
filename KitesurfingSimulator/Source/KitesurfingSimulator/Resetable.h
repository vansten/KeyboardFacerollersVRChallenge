// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Resetable.generated.h"

/**
 * 
 */
UINTERFACE()
class KITESURFINGSIMULATOR_API UResetable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IResetable
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual void ResetObject();
};