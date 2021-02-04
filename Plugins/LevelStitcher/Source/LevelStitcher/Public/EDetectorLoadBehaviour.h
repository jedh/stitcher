// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum EDetectorLoadBehaviour
{
    NONE,
    LOAD_AND_UNLOAD,
    LOAD_ONLY,
    UNLOAD_ONLY
};