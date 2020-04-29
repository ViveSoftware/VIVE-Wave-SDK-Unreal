// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRSplash.h"
#include "WaveVRPrivatePCH.h"
#include "WaveVRHMD.h"
#include "WaveVRRender.h"
#include "ClearQuad.h"

DEFINE_LOG_CATEGORY_STATIC(WaveSplash, Display, All);

FWaveVRSplash::FWaveVRSplash(FWaveVRRender* InRender)
	: bAutoShow(false)
	, SplashTexture(nullptr)
	, RendererModule(nullptr)
	, mRender(InRender)
	, BackGroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
	, ScaleFactor(1.0f)
	, PositionShift(FVector2D::ZeroVector)
	, bInitialized(false)
	, bIsShown(false)
	, bSplashScreenRendered(false)
{
	LOG_FUNC();
	static const FName RendererModuleName("Renderer");
	RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	check(RendererModule);
}

FWaveVRSplash::~FWaveVRSplash()
{
	LOG_FUNC();
	Hide();
	if (bInitialized)
	{
		FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
		FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	}
	DisApplyTexture();
	RendererModule = nullptr;
	FWaveVRAPIWrapper::GetInstance()->DelOverlay(SplashOverlayId);
	SplashOverlayId = -1;
}

void FWaveVRSplash::Init()
{
	LOG_FUNC();
	if (!bInitialized)
	{
		FCoreUObjectDelegates::PreLoadMap.AddSP(this, &FWaveVRSplash::OnPreLoadMap);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddSP(this, &FWaveVRSplash::OnPostLoadMap);
		bInitialized = true;
	}

	FWaveVRSplash * pWaveVRSplash = this;
	ENQUEUE_RENDER_COMMAND(AllocateSplashWrapper)(
		[pWaveVRSplash](FRHICommandListImmediate& RHICmdList)
		{
			pWaveVRSplash->AllocateSplashWrapper_RenderThread();
		});
}

void FWaveVRSplash::OnPreLoadMap(const FString&)
{
	LOG_FUNC();
	if (bAutoShow) {
		Show();
	}
}

void FWaveVRSplash::OnPostLoadMap(UWorld*)
{
	LOG_FUNC();
	if (bAutoShow) {
		Hide();
	}
}

void FWaveVRSplash::Show()
{
	LOG_FUNC();
	check(IsInGameThread());

	ApplyTexture();

	if (!mRender->IsInitialized()) {
		return;
	}

	if (SplashOverlayId < 0 || !FWaveVRAPIWrapper::GetInstance()->IsOverlayValid(SplashOverlayId)) {
		FWaveVRAPIWrapper::GetInstance()->GenOverlay(&SplashOverlayId);
		LOGD(WaveSplash, "FWaveVRAPIWrapper::GetInstance()->GenOverlay SplashOverlayId is (%u) ", SplashOverlayId);
	}

	if (bIsShown)
	{
		return;
	}

	if(bSplashScreenRendered) {
		uint32_t textureWidth = SplashWrapperRHIRef->GetSizeX();
		uint32_t textureHeight = SplashWrapperRHIRef->GetSizeY();
		const WVR_OverlayPosition position = {0.0f, 0.0f, -0.4f};
		const WVR_OverlayTexture_t texture = { (uint32_t) SplashWrapperId, textureWidth, textureHeight };
		LOGI(WaveSplash, "SplashTextureId(%u) textureWidth(%d) textureHeight(%d) SplashOverlayId(%u)", SplashWrapperId, textureWidth, textureHeight, SplashOverlayId);
		FWaveVRAPIWrapper::GetInstance()->SetOverlayTextureId(SplashOverlayId, &texture);
		FWaveVRAPIWrapper::GetInstance()->SetOverlayFixedPosition(SplashOverlayId, &position);
		FWaveVRAPIWrapper::GetInstance()->ShowOverlay(SplashOverlayId);
	}

	FWaveVRSplash * pWaveVRSplash = this;
	ENQUEUE_RENDER_COMMAND(SubmitSplash)(
		[pWaveVRSplash](FRHICommandListImmediate& RHICmdList)
			{
		pWaveVRSplash->SubmitFrame_RenderThread();
		});
	FlushRenderingCommands();

	bIsShown = true;
}

void FWaveVRSplash::Hide()
{
	LOG_FUNC();
	check(IsInGameThread());

	if (!bIsShown)
	{
		return;
	}

	DisApplyTexture();

	FWaveVRAPIWrapper::GetInstance()->HideOverlay(SplashOverlayId);
	bIsShown = false;
}

void FWaveVRSplash::RenderStereo_RenderThread()
{
	LOG_FUNC();
	check(IsInRenderingThread());
	if (!SplashWrapperRHIRef.IsValid()) {
		LOGD(WaveSplash, "RenderStereo_RenderThread : SplashWrapperRHIRef is not Initialized");
		return;
	}

	if (!bSplashScreenRendered)
	{
		FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();

		const uint32 ViewportWidth = SplashWrapperRHIRef->GetSizeX();
		const uint32 ViewportHeight = SplashWrapperRHIRef->GetSizeY();
		FIntRect DstRect = FIntRect(0, 0, ViewportWidth, ViewportHeight);
		RHICmdList.SetViewport(DstRect.Min.X, DstRect.Min.Y, 0, DstRect.Max.X, DstRect.Max.Y, 1.0f);

		//SetRenderTarget(RHICmdList, SplashWrapperRHIRef, FTextureRHIRef());
		FRHIRenderPassInfo RPInfo(SplashWrapperRHIRef, ERenderTargetActions::DontLoad_Store);
		RHICmdList.BeginRenderPass(RPInfo, TEXT("BlitSplashTexture"));
		{
			DrawClearQuad(RHICmdList, BackGroundColor);

			if (SplashTexture != nullptr && SplashTexture->IsValidLowLevel())
			{
				auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
				TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
				TShaderMapRef<FScreenPS> PixelShader(ShaderMap);
				FGraphicsPipelineStateInitializer GraphicsPSOInit;
				RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

				GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGBA, BO_Add, BF_SourceAlpha, BF_InverseSourceAlpha, BO_Add, BF_One, BF_InverseSourceAlpha>::GetRHI();
				GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None, true, false>::GetRHI();
				GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
				GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
				GraphicsPSOInit.PrimitiveType = PT_TriangleList;
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

				//Apply Splash texture texels
				PixelShader->SetParameters(RHICmdList, TStaticSamplerState<SF_Trilinear>::GetRHI(), SplashTexture->Resource->TextureRHI);

				float RenderOffsetX = PositionShift.X + (1.0f - ScaleFactor) * 0.5 * ViewportWidth;
				float RenderOffsetY = PositionShift.Y + (1.0f - ScaleFactor) * 0.5 * ViewportHeight;

				float RenderTargetSizeX = ViewportWidth * ScaleFactor;
				float RenderTargetSizeY = ViewportHeight * ScaleFactor;

				RendererModule->DrawRectangle(
					RHICmdList,
					RenderOffsetX, RenderOffsetY,         //top left corner of the quad
					RenderTargetSizeX, RenderTargetSizeY,
					0.0, 0.0,                              //U, V
					1.0, 1.0,                              //USize, VSize,
					FIntPoint(ViewportWidth, ViewportHeight),
					FIntPoint(1, 1),
					*VertexShader,
					EDRF_Default);
				}
			bSplashScreenRendered = true;
		}
		RHICmdList.EndRenderPass();
	}
}

bool FWaveVRSplash::ApplyTexture()
{
	LOG_FUNC();

	//TODO: Need make Texture UAsset to be cooked first if we use path to load UTexture2D.
	//SplashTexturePath = "/Game/Test.Test";
	//FSoftObjectPath Softload(SplashTexturePath);
	//SplashTexture = Cast<UTexture2D>(Softload.TryLoad());

	bool ret = false;
	if (SplashTexture != nullptr && SplashTexture->IsValidLowLevel()) {
		LOGD(WaveSplash, "Splash Texture is valid!");
		SplashTexture->AddToRoot();
		SplashTexture->UpdateResource();

		FWaveVRSplash * pWaveVRSplash = this;
		ENQUEUE_RENDER_COMMAND(ApplySplashTexture) (
			[pWaveVRSplash](FRHICommandListImmediate& RHICmdList)
			{
				pWaveVRSplash->bSplashScreenRendered = false;
				pWaveVRSplash->RenderStereo_RenderThread();
			});
		FlushRenderingCommands();
		ret = true;
	} else {
		LOGD(WaveSplash, "Splash Texture is invalid!");
	}
	return ret;
}


void FWaveVRSplash::DisApplyTexture()
{
	LOG_FUNC();
	if (SplashTexture != nullptr && SplashTexture->IsValidLowLevel())
	{
		SplashTexture->RemoveFromRoot();
		SplashTexture = nullptr;
	}
}

void FWaveVRSplash::SetSplashParam(UTexture2D* InSplashTexture, FLinearColor InBackGroundColor, float InScaleFactor, FVector2D Shift,bool EnableAutoLoading) {
	LOG_FUNC();
	BackGroundColor = InBackGroundColor;
	ScaleFactor = InScaleFactor;
	PositionShift = Shift;
	bAutoShow = EnableAutoLoading;
	SplashTexture = InSplashTexture;
}

bool FWaveVRSplash::AllocateSplashWrapper_RenderThread() {

	bool ret = false;

	auto OpenGLDynamicRHI = static_cast<FOpenGLDynamicRHI*>(GDynamicRHI);
	check(OpenGLDynamicRHI);
	uint32 InSizeX = mRender->GetSingleEyePixelWidth();
	uint32 InSizeY = mRender->GetSingleEyePixelHeight();
	EPixelFormat InFormat = EPixelFormat::PF_A2B10G10R10;
	uint32 InNumMips = 1;
	uint32 InNumSamples = 1;
	uint32 InFlags = 0;
	FRHIResourceCreateInfo CreateInfo;
	SplashWrapperRHIRef = OpenGLDynamicRHI->RHICreateTexture2D(InSizeX, InSizeY, (uint8)InFormat, InNumMips, InNumSamples, InFlags, CreateInfo);

	if (SplashWrapperRHIRef.IsValid()) {
		SplashWrapperId = static_cast<uint32_t>(*(GLuint*)SplashWrapperRHIRef->GetNativeResource());
		LOGI(WaveSplash, "SplashWrapperId (%u) ", SplashWrapperId);
		ret = true;
	} else {
		LOGD(WaveSplash, "SplashWrapperRHIRef is not valid!");
	}
	return ret;
}

void FWaveVRSplash::SubmitFrame_RenderThread() {
	check(mRender->IsInitialized());
	check(IsInRenderingThread());
	mRender->SubmitFrame_RenderThread();
}
