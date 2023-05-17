// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenCVUtils.h"

#include "PreOpenCVHeaders.h" // IWYU pragma: keep
#include <opencv2/core/mat.hpp>
#include <opencv2/aruco/dictionary.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "PostOpenCVHeaders.h" // IWYU pragma: keep

bool UOpenCVUtils::ConvertTextureToArray(UTexture* Texture, int& XSize, int& YSize, TArray<FColor>& OutPixels)
{
	if (!Texture || !Texture->GetResource() || !Texture->GetResource()->TextureRHI)
	{
		UE_LOG(LogTemp, Error, TEXT("Texture invalid"));
		return false;
	}

	FTexture2DRHIRef Texture2D = Texture->GetResource()->TextureRHI->GetTexture2D();
	if (!Texture2D)
	{
		UE_LOG(LogTemp, Error, TEXT("Texture2D invalid"));
		return false;
	}

	if (Texture2D->GetFormat() != PF_R8G8B8A8)
	{
		UE_LOG(LogTemp, Error, TEXT("Texture2D not support format %d"), Texture2D->GetFormat());
		return false;
	}

	XSize = Texture2D->GetSizeXY().X;
	YSize = Texture2D->GetSizeXY().Y;
	OutPixels.Init(FColor(0, 0, 0, 255), XSize * YSize);
	struct FReadSurfaceContext
	{
		FTexture2DRHIRef Texture;
		TArray<FColor>* OutData;
		FIntRect Rect;
		FReadSurfaceDataFlags Flags;
	};

	FReadSurfaceContext ReadSurfaceContext =
	{
		Texture2D,
		&OutPixels,
		FIntRect(0, 0, Texture2D->GetSizeXY().X, Texture2D->GetSizeXY().Y),
		FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
	};

	FReadSurfaceContext Context = ReadSurfaceContext;
	ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)(
		[Context](FRHICommandListImmediate& RHICmdList)
		{
			RHICmdList.ReadSurfaceData(
				Context.Texture,
				Context.Rect,
				*Context.OutData,
				Context.Flags
			);
		});

	FlushRenderingCommands();

	return true;
}

cv::Mat UOpenCVUtils::ConvetArrayToMat(int XSize, int YSize, const TArray<FColor>& Pixels)
{
	cv::Mat inputImage(YSize, XSize, CV_8UC3);

	for (int y = 0; y < YSize; y++)
	{
		for (int x = 0; x < XSize; x++)
		{
			int i = x + (y * XSize);
			inputImage.data[i * 3 + 0] = Pixels[i].B;
			inputImage.data[i * 3 + 1] = Pixels[i].G;
			inputImage.data[i * 3 + 2] = Pixels[i].R;
		}
	}

	return inputImage;
}

void UOpenCVUtils::DetectMarker(UTexture* Texture, FString OutPath)
{
	// ‘‚¢‚Ä‚ÍŒ©‚½‚à‚Ì‚ÌˆÈ‰º‚ðŽQÆ‚µ‚½•û‚ª‚æ‚³‚»‚¤
	// UE_5.2\Engine\Plugins\Runtime\OpenCV\Source\OpenCVHelper\Private\OpenCVBlueprintFunctionLibrary.cpp

	int SizeX, SizeY;
	TArray<FColor> Colors;
	if (!ConvertTextureToArray(Texture, SizeX, SizeY, Colors))
	{
		return;
	}


	cv::Mat inputImage = ConvetArrayToMat(SizeX, SizeY, Colors);

	std::vector<int> markerIds;
	std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

	cv::Ptr<cv::aruco::DetectorParameters> detectorParams = cv::aruco::DetectorParameters::create();
	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
	cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, detectorParams, rejectedCandidates);

	cv::aruco::drawDetectedMarkers(inputImage, markerCorners, markerIds);
	cv::imwrite(TCHAR_TO_UTF8(*OutPath), inputImage);
}