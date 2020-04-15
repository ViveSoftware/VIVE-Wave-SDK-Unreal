// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

//#pragma once

#include "WaveVRPrivatePCH.h"
#include "HeadMountedDisplay.h"
#include "WaveVRHMD.h"
#include "RendererPrivate.h"

static const float kVignetteHardness = 25;

inline void WaveVRDrawIndexedPrimitive(
  FRHICommandList& RHICmdList,
  uint32 PrimitiveType,
  uint32 MinVertexIndex,
  uint32 NumVertices,
  uint32 NumPrimitives,
  const void* IndexData,
  uint32 IndexDataStride,
  const void* VertexData,
  uint32 VertexDataStride )
{
	const uint32 NumIndices = GetVertexCountForPrimitiveCount( NumPrimitives, PrimitiveType );

	FRHIResourceCreateInfo CreateInfo;
	FVertexBufferRHIRef VertexBufferRHI = RHICreateVertexBuffer(VertexDataStride * NumVertices, BUF_Volatile, CreateInfo);
	void* VoidPtr = RHILockVertexBuffer(VertexBufferRHI, 0, VertexDataStride * NumVertices, RLM_WriteOnly);
	FPlatformMemory::Memcpy(VoidPtr, VertexData, VertexDataStride * NumVertices);
	RHIUnlockVertexBuffer(VertexBufferRHI);

	FIndexBufferRHIRef IndexBufferRHI = RHICreateIndexBuffer(IndexDataStride, IndexDataStride * NumIndices, BUF_Volatile, CreateInfo);
	void* VoidPtr2 = RHILockIndexBuffer(IndexBufferRHI, 0, IndexDataStride * NumIndices, RLM_WriteOnly);
	FPlatformMemory::Memcpy(VoidPtr2, IndexData, IndexDataStride * NumIndices);
	RHIUnlockIndexBuffer(IndexBufferRHI);

	RHICmdList.SetStreamSource(0, VertexBufferRHI, 0);
	RHICmdList.DrawIndexedPrimitive(IndexBufferRHI, PrimitiveType, MinVertexIndex, 0, NumVertices, 0, NumPrimitives, 1);

	IndexBufferRHI.SafeRelease();
	VertexBufferRHI.SafeRelease();
}

static void ComputeDitortion(EStereoscopicPass Eye, const FVector2D input, FVector2D * output)
{
	// input is in [0, 1]. Adjust it to [-1,1]
	const FVector2D zero(0.5f, 0.5f);
	FVector2D in = (input - zero) * 2;
	//distance to zero
	double r = in.Size();
	double r2 = r * r;
	double r4 = r2 * r2;

	const double Coefficient_R_K1 = 0.21;
	const double Coefficient_R_K2 = 0.21;
	const double Coefficient_G_K1 = 0.23;
	const double Coefficient_G_K2 = 0.23;
	const double Coefficient_B_K1 = 0.25;
	const double Coefficient_B_K2 = 0.25;

	output[0] = (in * (1 + Coefficient_R_K1 * r2 + Coefficient_R_K2 * r4)) / 2 + zero;
	output[1] = (in * (1 + Coefficient_G_K1 * r2 + Coefficient_G_K2 * r4)) / 2 + zero;
	output[2] = (in * (1 + Coefficient_B_K1 * r2 + Coefficient_B_K2 * r4)) / 2 + zero;
}

void FWaveVRHMD::GenerateDistortionCorrectionVertexBuffer(EStereoscopicPass Eye)
{
	LOG_FUNC();
	FDistortionVertex** UsingPtr = (Eye == eSSP_LEFT_EYE) ? &DistortionMeshVerticesLeftEye : &DistortionMeshVerticesRightEye;
	FDistortionVertex*& Verts = *UsingPtr;

	// Cleanup old data if necessary
	delete[] Verts;
	Verts = nullptr;

	// Allocate new vertex buffer
	Verts = new FDistortionVertex[NumVerts];

	uint32 VertexIndex = 0;
	for (uint32 y = 0; y < DistortionPointsY; ++y)
	{
		for (uint32 x = 0; x < DistortionPointsX; ++x)
		{
			FVector2D XYNorm = FVector2D(float(x) / float(DistortionPointsX - 1), float(y) / float(DistortionPointsY - 1));
			FVector2D DistortedCoords[3];
			FVector2D UnDistortedCoord = XYNorm;

			// Approximate the vertex position using the distortion function.
			for (uint32 i = 0; i < 10; ++i)
			{
				ComputeDitortion(Eye, UnDistortedCoord, DistortedCoords);
				//LOGI(LogWaveVRHMD, "Approx. %d:  input(%f, %f) G(%f,%f)", i, UnDistortedCoord.X, UnDistortedCoord.Y, DistortedCoords[1].X, DistortedCoords[1].Y);
				FVector2D Delta = XYNorm - DistortedCoords[1];
				if (Delta.Size() < 0.001f)
				{
					break;
				}
				if (i != 9)
				{
					UnDistortedCoord += Delta * 0.5f;
				}
			}
			float ScreenYDirection = -1.0f;

			const FVector2D ScreenPos = FVector2D(UnDistortedCoord.X * 2.0f - 1.0f, (UnDistortedCoord.Y * 2.0f - 1.0f) * ScreenYDirection);

			const FVector2D OrigRedUV = FVector2D(DistortedCoords[0].X, DistortedCoords[0].Y);
			const FVector2D OrigGreenUV = FVector2D(DistortedCoords[1].X, DistortedCoords[1].Y);
			const FVector2D OrigBlueUV = FVector2D(DistortedCoords[2].X, DistortedCoords[2].Y);

			// Final distorted UVs
			FVector2D FinalRedUV = OrigRedUV;
			FVector2D FinalGreenUV = OrigGreenUV;
			FVector2D FinalBlueUV = OrigBlueUV;

			float Vignette = FMath::Clamp(XYNorm.X * kVignetteHardness, 0.0f, 1.0f)
				* FMath::Clamp((1 - XYNorm.X)* kVignetteHardness, 0.0f, 1.0f)
				* FMath::Clamp(XYNorm.Y * kVignetteHardness, 0.0f, 1.0f)
				* FMath::Clamp((1 - XYNorm.Y)* kVignetteHardness, 0.0f, 1.0f);

			FDistortionVertex FinalVertex = FDistortionVertex {ScreenPos, FinalRedUV, FinalGreenUV, FinalBlueUV, Vignette, 0.0f};
			Verts[VertexIndex++] = FinalVertex;
		}
	}

	check(VertexIndex == NumVerts);
}


void FWaveVRHMD::GenerateDistortionCorrectionIndexBuffer()
{
	LOG_FUNC();

	// Delete existing indices if they exist
	delete[] DistortionMeshIndices;
	DistortionMeshIndices = nullptr;

	// Allocate new indices
	DistortionMeshIndices = new uint16[6 * DistortionPointsX * DistortionPointsY];

	uint32 InsertIndex = 0;
	for (uint32 y = 0; y < DistortionPointsY - 1; ++y)
	{
		for (uint32 x = 0; x < DistortionPointsX - 1; ++x)
		{
			// Calculate indices for the triangle
			const uint16 BottomLeft = (y * DistortionPointsX) + x + 0;
			const uint16 BottomRight = (y * DistortionPointsX) + x + 1;
			const uint16 TopLeft = (y * DistortionPointsX) + x + 0 + DistortionPointsX;
			const uint16 TopRight = (y * DistortionPointsX) + x + 1 + DistortionPointsX;

			// Insert indices
			DistortionMeshIndices[InsertIndex + 0] = BottomLeft;
			DistortionMeshIndices[InsertIndex + 1] = BottomRight;
			DistortionMeshIndices[InsertIndex + 2] = TopRight;
			DistortionMeshIndices[InsertIndex + 3] = BottomLeft;
			DistortionMeshIndices[InsertIndex + 4] = TopRight;
			DistortionMeshIndices[InsertIndex + 5] = TopLeft;
			InsertIndex += 6;
		}
	}

	check(InsertIndex == NumIndices);
}

void FWaveVRHMD::SetNumOfDistortionPoints(int32 XPoints, int32 YPoints)
{
	LOG_FUNC();

	// clamp values
	if (XPoints < 2)
	{
		XPoints = 2;
	}
	else if (XPoints > 200)
	{
		XPoints = 200;
	}

	if (YPoints < 2)
	{
		YPoints = 2;
	}
	else if (YPoints > 200)
	{
		YPoints = 200;
	}

	// calculate our values
	DistortionPointsX = XPoints;
	DistortionPointsY = YPoints;
	NumVerts = DistortionPointsX * DistortionPointsY;
	NumTris = (DistortionPointsX - 1) * (DistortionPointsY - 1) * 2;
	NumIndices = NumTris * 3;

	// generate the distortion mesh
	GenerateDistortionCorrectionIndexBuffer();
	GenerateDistortionCorrectionVertexBuffer(eSSP_LEFT_EYE);
	GenerateDistortionCorrectionVertexBuffer(eSSP_RIGHT_EYE);
}

void FWaveVRHMD::GetEyeRenderParams_RenderThread(const struct FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const
{
	LOG_FUNC();
	if (Context.View.StereoPass == eSSP_LEFT_EYE)
	{
		EyeToSrcUVOffsetValue.X = 0.0f;
		EyeToSrcUVOffsetValue.Y = 0.0f;

		EyeToSrcUVScaleValue.X = 0.5f;
		EyeToSrcUVScaleValue.Y = 1.0f;
	}
	else
	{
		EyeToSrcUVOffsetValue.X = 0.5f;
		EyeToSrcUVOffsetValue.Y = 0.0f;

		EyeToSrcUVScaleValue.X = 0.5f;
		EyeToSrcUVScaleValue.Y = 1.0f;
	}
}

void FWaveVRHMD::DrawDistortionMesh_RenderThread(struct FRenderingCompositePassContext& Context, const FIntPoint& TextureSize)
{
	LOG_FUNC();

	const FSceneView& View = Context.View;
	FRHICommandListImmediate& RHICmdList = Context.RHICmdList;
	const FSceneViewFamily& ViewFamily = *(View.Family);
	FIntPoint ViewportSize = ViewFamily.RenderTarget->GetSizeXY();

	//LOGI(LogWaveVRHMD, "StereoPass: %d ViewportSize (%d, %d) TextureSize (%d, %d)"), (int)View.StereoPass, ViewportSize.X, ViewportSize.Y, TextureSize.X, TextureSize.Y);

	if (View.StereoPass == eSSP_LEFT_EYE) {
		RHICmdList.SetViewport(0, 0, 0.0f, ViewportSize.X / 2, ViewportSize.Y, 1.0f);
		WaveVRDrawIndexedPrimitive(Context.RHICmdList, PT_TriangleList, 0, NumVerts, NumTris, DistortionMeshIndices,
			sizeof(DistortionMeshIndices[0]), DistortionMeshVerticesLeftEye, sizeof(DistortionMeshVerticesLeftEye[0]));
	} else if (View.StereoPass == eSSP_RIGHT_EYE) {
		RHICmdList.SetViewport(ViewportSize.X / 2, 0, 0.0f, ViewportSize.X, ViewportSize.Y, 1.0f);
		WaveVRDrawIndexedPrimitive(Context.RHICmdList, PT_TriangleList, 0, NumVerts, NumTris, DistortionMeshIndices,
			sizeof(DistortionMeshIndices[0]), DistortionMeshVerticesRightEye, sizeof(DistortionMeshVerticesRightEye[0]));
	}
}
