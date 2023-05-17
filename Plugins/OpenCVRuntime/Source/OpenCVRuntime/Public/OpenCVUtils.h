// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OpenCVUtils.generated.h"

namespace cv
{
	struct Mat;
}


USTRUCT(BlueprintType)
struct OPENCVRUNTIME_API FCVPoint2F
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		float X;

	UPROPERTY(BlueprintReadWrite)
		float Y;
};

USTRUCT(BlueprintType)
struct OPENCVRUNTIME_API FMarkers
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int Id;

	UPROPERTY(BlueprintReadWrite)
	TArray<FCVPoint2F> Corners;
};

/**
 * 
 */
UCLASS()
class OPENCVRUNTIME_API UOpenCVUtils : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static void DetectMarker(UTexture* Texture, FString OutPath);

private:
	static bool ConvertTextureToArray(UTexture* Texture, int& XSize, int& YSize, TArray<FColor>& OutPixels);
	static cv::Mat ConvetArrayToMat(int XSize, int YSize, const TArray<FColor>& Pixels);
};
