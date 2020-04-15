// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRPrivatePCH.h"

#include "Logging/LogMacros.h"

#include "WaveVRHMD.h"
#include "WaveVRRender.h"
#include "RHIUtilities.h"
#include "OpenGLResources.h"
#include "XRThreadUtils.h"

#if (UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG) && !WITH_EDITOR
#define WVR_SCOPED_NAMED_EVENT(name, color) SCOPED_NAMED_EVENT(name, color)
#else
#define WVR_SCOPED_NAMED_EVENT(name, color)
#endif

#if PLATFORM_ANDROID && PLATFORM_ANDROID_ARM //(32bit)
#define PTR_TO_INT(x) x
#define INT_TO_PTR(x) x
#else //PLATFORM_ANDROID_ARM64 || PLATFORM_WINDOWS || WITH_EDITOR  /*Other 64bit*/
#define PTR_TO_INT(x) \
  (int)(long long)(x)
#define INT_TO_PTR(x) \
  (long long)(x)
#endif

#if PLATFORM_ANDROID || PLATFORM_WINDOWS
#if WAVEVR_LOG_SHOW_ALL_ENTRY && !WITH_EDITOR
#undef LOG_FUNC
#define LOG_FUNC() LOGD(WVRRender, "%s", WVR_FUNCTION_STRING) // ;FDebug::DumpStackTraceToLog();
#endif
#define LOG_STACKTRACE() FDebug::DumpStackTraceToLog()
#endif //PLATFORM_ANDROID

/**
  MSAA and MultiView configuration TruthTable
  T for work, S for work and msaa4x, F for all fail.

  Multiview
    0  1   <- use Unreal queue
  0 F  F   Note: WVR api didn't accept/submit the multiview resources created from Unreal.
  1 S  S
  ^- create from resource

  No Multiview
    0  1   <- use Unreal queue
  0 S  S   Note: (0,0) is not smooth.  Maybe the queue order is not follow SVR's order.
  1 S  T
  ^- create from resource
 **/
#if 0  // Set 1 for internal test.  It will override the plugin settings.
#define WVRRENDER_USE_UNREAL_TEXTURE_QUEUE(input) false
#define WVRRENDER_CREATE_FROM_RESOURCE(input) true
#else
#define WVRRENDER_USE_UNREAL_TEXTURE_QUEUE(input) input
#define WVRRENDER_CREATE_FROM_RESOURCE(input) input
#endif

DEFINE_LOG_CATEGORY_STATIC(WVRRender, Display, All);

#if PLATFORM_ANDROID
#include <android_native_app_glue.h>
extern struct android_app* GNativeAndroidApp;
#endif // PLATFORM_ANDROID

void ReportRenderError(WVR_RenderError render_error);

#define LogTextureInfo(title, info) \
	LOGI(WVRRender, title " TextureInfo width=%u height=%u array=%u samples=%u flags=%u extFlags=%u " \
		"format=%u mips=%d target=%d capacity=%u useUnrealTextureQueue=%d createFromResource=%d queue=%p", \
		info.width, info.height, info.arraySize, info.numSamples, info.flags, info.extraFlags, info.format, info.mips, \
		info.target, info.capacity, info.useUnrealTextureQueue, info.createFromResource, info.wvrTextureQueue);


/****************************************************
 *
 * FWaveVRFXRRenderBridge
 *
 ****************************************************/

FWaveVRFXRRenderBridge::FWaveVRFXRRenderBridge(FWaveVRRender * render) :
	FXRRenderBridge(), mRender(render)
{
	LOG_FUNC();
}

FWaveVRFXRRenderBridge::~FWaveVRFXRRenderBridge()
{
	LOG_FUNC();
}

bool FWaveVRFXRRenderBridge::NeedsNativePresent()
{
	LOG_FUNC();
	return true; //D3D12 only now, not mobile.
}

bool FWaveVRFXRRenderBridge::Present(int& SyncInterval)
{
	LOG_FUNC();
	check(IsInRenderingThread());

	// Disable Unreal's VSync
	SyncInterval = 0;

	mRender->OnFinishRendering_RenderThread();

	// Not to swap
	return false;
}

void FWaveVRFXRRenderBridge::OnBackBufferResize()
{
	LOG_FUNC();
}


/****************************************************
 *
 * FWaveVRTexturePool
 *
 ****************************************************/

class FWaveVRTexturePool
{
public:
	// Create base texture
	FWaveVRTexturePool(const FWaveVRRenderTextureInfo& info);

	virtual ~FWaveVRTexturePool();
	void ReleaseTextures();

public:
	TRefCountPtr<FOpenGLTexture2D> CreateBaseTexture();
	void CreateTextureInPool(uint32 index);

public:
	void MakeAlias(int index, GLuint resource = 0);
	uint32 FindIndexByGLId(GLuint id) const;
	uint32 FindIndexByResource(void * resource) const;
	uint32 FindIndexByTexture(void * textureBaseRHI) const;

public:
	// Get
	inline const FWaveVRRenderTextureInfo GetInfo() const { return textureInfo; }
	const TRefCountPtr<FOpenGLTexture2D> GetGLTexture(uint32 index) const;
	FTexture2DRHIRef GetRHITexture(uint32 index) const;

private:
	FWaveVRTexturePool(const FWaveVRTexturePool &) = delete;
	FWaveVRTexturePool(FWaveVRTexturePool &&) = delete;
	FWaveVRTexturePool &operator=(const FWaveVRTexturePool &) = delete;

private:
	TRefCountPtr<FOpenGLTexture2D> mBaseTexture;

	FTextureRHIRef * texturePoolRHIRefs;

	FWaveVRRenderTextureInfo textureInfo;
};

FWaveVRTexturePool::FWaveVRTexturePool(const FWaveVRRenderTextureInfo& info) :
	mBaseTexture(nullptr), texturePoolRHIRefs(nullptr), textureInfo(info)
{
	LOG_FUNC();
	LogTextureInfo("FWaveVRTexturePool", info);

	// This is a pool.
	check(textureInfo.capacity);

	texturePoolRHIRefs = new FTextureRHIRef[textureInfo.capacity];
	for (int i = 0; i < textureInfo.capacity; i++)
		texturePoolRHIRefs[i] = nullptr;
}

FWaveVRTexturePool::~FWaveVRTexturePool()
{
	LOG_FUNC();
	ReleaseTextures();
}

// Used with old unreal WVR texture queue.  Not follow WVR's texture queue
TRefCountPtr<FOpenGLTexture2D> FWaveVRTexturePool::CreateBaseTexture()
{
	LOG_FUNC();

	check(!textureInfo.useUnrealTextureQueue);
	auto GLRHI = static_cast<FOpenGLDynamicRHI*>(GDynamicRHI);
	check(GLRHI);

	// Create base texture which will give to SceneViewport of UnrealEngine.
	mBaseTexture = new FOpenGLTexture2D(GLRHI, 0, textureInfo.target, GL_NONE/*GL_COLOR_ATTACHMENT0*/, textureInfo.width, textureInfo.height, 0, 1, textureInfo.numSamples, textureInfo.numSamples, textureInfo.arraySize, EPixelFormat(textureInfo.format), false, false, textureInfo.flags, nullptr, FClearValueBinding::Black);
	mBaseTexture->SetAliased(true);
	OpenGLTextureAllocated(mBaseTexture, textureInfo.flags);

	LOGD(WVRRender, "mBaseTexture %u", mBaseTexture->Resource);

	return mBaseTexture;
}

static FTexture2DRHIRef CreateTexture2D(const FWaveVRRenderTextureInfo& info, GLuint resource = 0)
{
	LOG_FUNC();
	if (PLATFORM_WINDOWS) {
		FRHIResourceCreateInfo createInfo;
		return GDynamicRHI->RHICreateTexture2D(
			info.width, info.height, EPixelFormat(info.format),
			1/*Mips*/, info.numSamples, info.flags, createInfo);
	} else if (PLATFORM_ANDROID) {
		auto GLRHI = static_cast<FOpenGLDynamicRHI*>(GDynamicRHI);
		check(GLRHI);

		if (info.arraySize > 1) {
			if (resource == 0) {
				// Not Work ?  texture release it self?  modify the texture as FTexture2DRHIRef.  Need verify.
				FTexture2DRHIRef texture = new FOpenGLTexture2D(
					GLRHI, 0, info.target, GL_NONE, info.width, info.height, 0,
					info.mips, info.numSamples, info.numSamples/*SamplesTileMem*/,
					info.arraySize, EPixelFormat(info.format), false, false, info.flags, nullptr, FClearValueBinding::Black);
				OpenGLTextureAllocated(texture, info.flags);
				GLRHI->InitializeGLTexture(
					texture, info.width, info.height, false, true, false,
					EPixelFormat(info.format), info.mips, 1/*numSamples*/,
					info.arraySize, info.flags, FClearValueBinding::Black, nullptr);
				return texture;
			} else {
				// Work
				return GLRHI->RHICreateTexture2DArrayFromResource(
					EPixelFormat(info.format), info.width, info.height,
					info.arraySize,
					1/*Mips*/, info.numSamples, info.numSamples/*SamplesTileMem*/,
					FClearValueBinding::Black, resource, info.flags);
			}
		} else {
			if (resource == 0) {
				FRHIResourceCreateInfo createInfo;
				return GDynamicRHI->RHICreateTexture2D(
					info.width, info.height, EPixelFormat(info.format),
					1/*Mips*/, info.numSamples, info.flags, createInfo);
			} else {
				// TODO not Work
				return GLRHI->RHICreateTexture2DFromResource(
					EPixelFormat(info.format), info.width, info.height,
					1/*Mips*/, info.numSamples, info.numSamples/*SamplesTileMem*/,
					FClearValueBinding::Black, resource, info.flags);
			}
		}
	}
}

// Only used to create the textures for WVR submit.  Textures are following the WVR settings.
void FWaveVRTexturePool::CreateTextureInPool(uint32 index)
{
	LOG_FUNC();

	check(texturePoolRHIRefs);
	check(index <= textureInfo.capacity);

	const auto info = textureInfo;

	// 4 cases
	if (textureInfo.useUnrealTextureQueue && textureInfo.createFromResource) {
		// Use textures in wvrTextureQueue and not use a base texture to alias.
		check(textureInfo.wvrTextureQueue);
		WVR_TextureParams_t params = WVR()->GetTexture(info.wvrTextureQueue, index);
		LOGI(WVRRender, "WVR_TextureParams_t[%d]=%p", index, params.id);

		texturePoolRHIRefs[index] = CreateTexture2D(info, (GLuint)PTR_TO_INT(params.id));
	} else if (!textureInfo.useUnrealTextureQueue && textureInfo.createFromResource) {
		// Use textures in wvrTextureQueue and use a base texture to alias.
		check(textureInfo.wvrTextureQueue);
		check(mBaseTexture);
		check(index == 0);

		for (int i = 0; i < info.capacity; i++) {
			WVR_TextureParams_t params = WVR()->GetTexture(info.wvrTextureQueue, info.capacity - 1 - i /* damn it. wvr is inversed */);
			LOGI(WVRRender, "WVR_TextureParams_t[%d]=%p", info.capacity - 1 - i, params.id);

			texturePoolRHIRefs[i] = CreateTexture2D(info, (GLuint)PTR_TO_INT(params.id));
		}
	} else if (!textureInfo.useUnrealTextureQueue && !textureInfo.createFromResource) {
		// As the old way
		// Create texture in unreal and use a base texture to alias.
		check(mBaseTexture);
		check(index == 0);

		for (int i = 0; i < info.capacity; i++) {
			texturePoolRHIRefs[i] = CreateTexture2D(info);
			MakeAlias(i);
		}
		//void ** textures = new void *[info.capacity];
		//for (int i = 0; i < info.capacity; i++) {
		//	textures[i] = (void *)PTR_TO_INT(*(GLuint *)(texturePoolRHIRefs[i]->GetNativeResource()));
		//	LOGI(WVRRender, "WVR_TextureParams_t[%d]=%p", i, textures[i]);
		//}
		//textureInfo.wvrTextureQueue = WVR()->StoreRenderTextures(textures, info.capacity, WVR_Eye_Left);

	} else if (textureInfo.useUnrealTextureQueue && !textureInfo.createFromResource) {
		// Create texture in unreal and not use a base texture to alias.
		texturePoolRHIRefs[index] = CreateTexture2D(info);
	} else {
		// Impossible
		check(false);
	}
}

// Very confusable here.
// Idx could be used by the target or the base.  It's depended on resource.
void FWaveVRTexturePool::MakeAlias(int idx, GLuint resource)
{
	LOG_FUNC();
	check(texturePoolRHIRefs);
	check(texturePoolRHIRefs[idx]);

	auto target = static_cast<FOpenGLTextureBase*>(texturePoolRHIRefs[idx]->GetTextureBaseRHI());
	auto base = mBaseTexture ? mBaseTexture : target;
	// mBaseTexture of course is aliased.  target could be aliased or not.
	if (base->IsAliased()) {
		if (resource == 0) {
			// Has mBase and use idx target.
			// Alias the the target resource from texture pool
			base->AliasResources(target);
			//LOGV(WVRRender, "MakeAlias(%d, 0) -> target %u", idx, target->Resource);
		} else {
			// For using unreal texture queue, the resource not match the wvr queue's idx.
			if (base->Resource != resource) {
				//LOGV(WVRRender, "MakeAlias(%d, %u) -> originalResource %u -> %u", idx, resource, base->Resource, resource);
				base->Resource = resource;
			} else {
				//LOGV(WVRRender, "MakeAlias(%d, %u) noop", idx, resource);
			}
		}
	} else {
		// No mBase and target is not aliased
		// Do nothing
		//LOGV(WVRRender, "MakeAlias(%d, %u) noop", idx, resource);
	}
}

uint32 FWaveVRTexturePool::FindIndexByGLId(GLuint id) const
{
	check(id);

	// Find the index
	for (int i = 0; i < textureInfo.capacity; i++) {
		auto base = (FOpenGLTextureBase*) texturePoolRHIRefs[i]->GetTextureBaseRHI();
		if (base->Resource == id) {
			return i;
		}
	}

	// Imposible
	LOGD(WVRRender, "FindIndexByGLId(id=%u) texture was not found in pool", id);
	for (int i = 0; i < textureInfo.capacity; i++) {
		auto base = (FOpenGLTextureBase*)texturePoolRHIRefs[i]->GetTextureBaseRHI();
		LOGD(WVRRender, "  texturePoolRHIRefs[%d]=%u", i, base->Resource);
	}
	check(false);
	return 0;
}

uint32 FWaveVRTexturePool::FindIndexByResource(void * nativeResource) const
{
	check(nativeResource);

	// Find the index
	for (int i = 0; i < textureInfo.capacity; i++) {
		check(texturePoolRHIRefs[i]);
		if (texturePoolRHIRefs[i]->GetNativeResource() == nativeResource) {
			return i;
		}
	}
	// Imposible
	check(false);
	return 0;
}

uint32 FWaveVRTexturePool::FindIndexByTexture(void * textureBaseRHI) const
{
	check(textureBaseRHI);

	// Find the index
	for (int i = 0; i < textureInfo.capacity; i++) {
		check(texturePoolRHIRefs[i]);
		if (texturePoolRHIRefs[i]->GetTextureBaseRHI() == textureBaseRHI) {
			return i;
		}
	}
	// Imposible
	check(false);
	return 0;
}

const TRefCountPtr<FOpenGLTexture2D> FWaveVRTexturePool::GetGLTexture(uint32 index) const
{
	if (mBaseTexture) {
		return mBaseTexture;
	} else {
		check(index < textureInfo.capacity);
		check(texturePoolRHIRefs[index]);
		return static_cast<FOpenGLTexture2D*>(texturePoolRHIRefs[index]->GetTextureBaseRHI());
	}
}

FTexture2DRHIRef FWaveVRTexturePool::GetRHITexture(uint32 index) const
{
	LOG_FUNC();
	if (mBaseTexture) {
		return mBaseTexture->GetTexture2D();
	} else {
		check(index < textureInfo.capacity);
		check(texturePoolRHIRefs[index]);
		return texturePoolRHIRefs[index]->GetTexture2D();
	}
}

void FWaveVRTexturePool::ReleaseTextures()
{
	bool hasBaseTexture = !!mBaseTexture;
	LOG_FUNC();
	if (texturePoolRHIRefs) {
		for (int i = 0; i < textureInfo.capacity; i++) {
			LOGD(WVRRender, "TexturePool: Release pool texture %d", i);
			// We dont really release the texture that we gave to engine.  Release it will cause an exception.
			//if (hasBaseTexture && texturePoolRHIRefs[i])
			//	texturePoolRHIRefs[i]->Release(); // Crash... I don't know why
			texturePoolRHIRefs[i].SafeRelease();
		}
		delete[] texturePoolRHIRefs;
		texturePoolRHIRefs = nullptr;
	}

	if (mBaseTexture) {
		LOGD(WVRRender, "TexturePool: Release Base texture");
		// We dont really release the texture that we gave to engine.  Release it will cause an exception.
		//mBaseTexture->Release();
		mBaseTexture.SafeRelease();
	}
	if (textureInfo.wvrTextureQueue) {
		LOGD(WVRRender, "TexturePool: Release wvrTextureQueue");
		WVR()->ReleaseTextureQueue(textureInfo.wvrTextureQueue);
		textureInfo.wvrTextureQueue = nullptr;
	}

	LOGD(WVRRender, "TexturePool: Released");
}


/****************************************************
 *
 * FWaveVRTextureManager
 *
 ****************************************************/


FWaveVRTextureManager::FWaveVRTextureManager(FWaveVRRender * render, FWaveVRHMD * hmd) :
	mColorTexturePool(nullptr),
	mCurrentResource(0),
	mCurrentResourceRT(0),
	mCurrentWVRTextureQueueIndex(0),
	mRender(render),
	mHMD(hmd)
{
	LOG_FUNC();
}

FWaveVRTextureManager::~FWaveVRTextureManager()
{
	LOG_FUNC();
	CleanTextures();
}

// Called by AllocateRenderTargetTexture
bool FWaveVRTextureManager::CreateColorTexturePool(const FWaveVRRenderTextureInfo& info)
{
	LOG_FUNC();
	LogTextureInfo("CreateColorTextures", info);

	check(!mColorTexturePool);

	// Init
	FWaveVRRenderTextureInfo infoCopy = info;

	if (info.arraySize > 1)
		wvrTextureTarget = WVR_TextureTarget_2D_ARRAY;  // Used when submit
	else
		wvrTextureTarget = WVR_TextureTarget_2D_DUAL;  // Used when submit

	if (info.useUnrealTextureQueue && info.createFromResource)
		infoCopy.useWVRTextureQueue = false;
	else if (!info.useUnrealTextureQueue && !info.createFromResource)
		infoCopy.useWVRTextureQueue = false;
	else
		infoCopy.useWVRTextureQueue = true;

	if (info.useUnrealTextureQueue && !info.createFromResource) {
	} else {
		if (wvrTextureTarget == WVR_TextureTarget_2D_DUAL)
			infoCopy.wvrTextureQueue = WVR()->ObtainTextureQueue(WVR_TextureTarget_2D, WVR_TextureFormat_RGBA, WVR_TextureType_UnsignedByte, info.width, info.height, 0);
		else  // WVR_TextureTarget_2D_ARRAY
			infoCopy.wvrTextureQueue = WVR()->ObtainTextureQueue(WVR_TextureTarget_2D_ARRAY, WVR_TextureFormat_RGBA, WVR_TextureType_UnsignedByte, info.width, info.height, 0);
	}
	infoCopy.capacity = info.wvrTextureQueue ? WVR()->GetTextureQueueLength(info.wvrTextureQueue) : infoCopy.capacity;

	// Create pool
	mColorTexturePool = new FWaveVRTexturePool(infoCopy);

	if (!infoCopy.useUnrealTextureQueue) {
		if (infoCopy.createFromResource) {
			mColorTexturePool->CreateBaseTexture();
		} else {
			mColorTexturePool->CreateBaseTexture();
		}
	}
	else {
		// If useUnrealTextureQueue, no need to create a base texture, and each texture will be allocated during AllocateColorTexture.
	}
	return true;
}

// Called by Render::AllocateRenderTargetTexture
FTexture2DRHIRef FWaveVRTextureManager::AllocateColorTexture(const FWaveVRRenderTextureInfo& engineInfo, uint32 index)
{
	LOG_FUNC();
	if (!mColorTexturePool) {
		LOGD(WVRRender, "Without ColorTexturePool");
		return nullptr;
	}

	auto textureInfo = mColorTexturePool->GetInfo();

	// check if we should create the texture for engine
	bool match =
		textureInfo.width == engineInfo.width &&
		textureInfo.height == engineInfo.height &&
		textureInfo.mips == textureInfo.mips;

	if (!match) {
		LOGD(WVRRender, "The engineInfo not match our requirement.");
		LogTextureInfo("AllocateColorTexture", engineInfo);
		return nullptr;
	}

	mColorTexturePool->CreateTextureInPool(index);

	return mColorTexturePool->GetRHITexture(index);
}

// Called by Render::AllocateDepthTexture
FTexture2DRHIRef FWaveVRTextureManager::AllocateDepthTexture(const FWaveVRRenderTextureInfo& engineInfo, uint32 index)
{
	LOG_FUNC();
	LogTextureInfo("AllocateDepthTexture", engineInfo);
	uint32 width = mRender->GetSingleEyePixelWidth(), height = mRender->GetSingleEyePixelHeight();

	if (engineInfo.width != width || engineInfo.height != height) {
		// Unknown size of depth
		LOGD(WVRRender, "engineInfo.width%d != width%d || engineInfo.height%d != height%d", engineInfo.height, width, engineInfo.height, height);
		return nullptr;
	}

	FWaveVRRenderTextureInfo depthInfo = {0};

	//if (!bInitialized)
	//{
	//	depthInfo = info;

	//	//bool followWVR = true;
	//	//if ((info.extraFlags & TexCreate_DepthStencilTargetable) == 0)
	//	//	followWVR = false;
	//	depthInfo.width = info.width;
	//	depthInfo.height = info.height;
	//	depthInfo.arraySize = isMultiViewEnabled ? 2 : 1;
	//	depthInfo.numSamples = info.numSamples;
	//	depthInfo.target = isMultiViewEnabled ? GL_TEXTURE_2D_ARRAY : (info.numSamples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
	//	depthInfo.format = PF_DepthStencil;  // ignore PF_X24_G8
	//	depthInfo.flags = info.flags | info.extraFlags;  // TexCreate_DepthStencilTargetable
	//}
	//else
	//{
	//	// Need to know if we really need depth buffer.
	//	//if (msaaLevel < 2)
	//	//	return false;

	//	depthInfo.width = isMultiViewEnabled ? width : width * 2;
	//	depthInfo.height = height;
	//	depthInfo.arraySize = isMultiViewEnabled ? 2 : 1;
	//	depthInfo.numSamples = msaaLevel;
	//	depthInfo.target = isMultiViewEnabled ? GL_TEXTURE_2D_ARRAY : (msaaLevel > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
	//	depthInfo.format = PF_DepthStencil;
	//	depthInfo.flags = TexCreate_DepthStencilTargetable | TexCreate_SRGB;
	//}

	depthInfo = engineInfo;

	depthInfo.arraySize = mRender->GetMultiView() ? 2 : 1;
	depthInfo.numSamples =  mRender->GetMultiSampleLevel();
	// In OpengGLRenderTarget.cpp: Depth should be GL_TEXTURE_2D_ARRAY
	depthInfo.target = mRender->GetMultiView() ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
	depthInfo.format = PF_DepthStencil;  // ignore PF_X24_G8
	depthInfo.flags = engineInfo.flags | engineInfo.extraFlags | TexCreate_DepthStencilTargetable;
	depthInfo.capacity = 0;
	depthInfo.createFromResource = false;
	depthInfo.useUnrealTextureQueue = false;
	depthInfo.wvrTextureQueue = nullptr;

	LogTextureInfo("depthInfo", depthInfo);

	auto OpenGLDynamicRHI = static_cast<FOpenGLDynamicRHI*>(GDynamicRHI);
	check(OpenGLDynamicRHI);

	auto texture = new FOpenGLTexture2D(
		OpenGLDynamicRHI, 0, depthInfo.target, GL_DEPTH_STENCIL_ATTACHMENT, depthInfo.width, depthInfo.height, 0,
		depthInfo.mips, 1/*numSamples*/, 1/*SamplesTileMem*/,
		depthInfo.arraySize, EPixelFormat(depthInfo.format), false, false, depthInfo.flags, nullptr, FClearValueBinding::Black);

	OpenGLTextureAllocated(texture, depthInfo.flags);
	OpenGLDynamicRHI->InitializeGLTexture(
		texture, depthInfo.width, depthInfo.height, false, true, false,
		EPixelFormat(depthInfo.format), depthInfo.mips, 1/*numSamples*/,
		depthInfo.arraySize, depthInfo.flags, FClearValueBinding::Black, nullptr);

	return texture;
}

// Can be invoked in both thread.
void FWaveVRTextureManager::Next(const FViewport& Viewport)
{
	WVR_SCOPED_NAMED_EVENT(TextMngrNext, FColor::Purple);

	LOG_FUNC();
	bool isInRenderingThread = IsInRenderingThread();

	if (!mColorTexturePool) {
		LOGD(WVRRender, "mColorTexturePool is nullptr");
		return;
	}

	auto info = mColorTexturePool->GetInfo();
	auto renderTarget = Viewport.GetRenderTargetTexture();
	check(renderTarget);

	if (PLATFORM_WINDOWS)	{
		//if (!info.wvrTextureQueue || !info.useWVRTextureQueue)  // Always true in windows?
		mCurrentResource = renderTarget->GetNativeResource();
	} else if (PLATFORM_ANDROID) {
		int idx = -1;
		if (!info.wvrTextureQueue || !info.useWVRTextureQueue) {
			// Without texture queue, find the queue our self.
			GLuint id = ((FOpenGLTextureBase*)renderTarget->GetTextureBaseRHI())->Resource;
			mCurrentResource = (void *) INT_TO_PTR(id);
			idx = mColorTexturePool->FindIndexByGLId(id);
			idx = (++idx) % info.capacity;
			mColorTexturePool->MakeAlias(idx);
			//LOGV(WVRRender, "thread(%d) wvrTextureQueue=%d index=%d, mCurrentResource=%u", isInRenderingThread ? 1 : 0, !!info.wvrTextureQueue, idx, id);
		} else {
			idx = WVR()->GetAvailableTextureIndex(info.wvrTextureQueue);
			//LOGV(WVRRender, "next texture index = %d", idx);

			// No available texture!
			if (idx < 0) {
				LOGE(WVRRender, "No available texture %d.  Should fix it.", idx);
				mCurrentWVRTextureQueueIndex = ++mCurrentWVRTextureQueueIndex % info.capacity;
				idx = mCurrentWVRTextureQueueIndex;
			} else {
				mCurrentWVRTextureQueueIndex = idx;
			}
			checkf(idx >= 0 && idx < info.capacity, TEXT("RenderTexture=%d is out of range."), idx);

			WVR_TextureParams_t params = WVR()->GetTexture(info.wvrTextureQueue, idx);
			mCurrentResource = params.id;

			//TRefCountPtr<FOpenGLTexture2D> openglRenderTarget = static_cast<FOpenGLTexture2D*>();
			//check(openglRenderTarget);
			GLuint id = (GLuint)PTR_TO_INT(mCurrentResource);
			uint32 foundIdx = mColorTexturePool->FindIndexByGLId(id);
			if (foundIdx != idx) {
				mColorTexturePool->MakeAlias(foundIdx, id);
			} else {
				mColorTexturePool->MakeAlias(idx);
			}

			//LOGV(WVRRender, "thread(%d) index=%d, mCurrentResource=%u", isInRenderingThread ? 1 : 0, idx, mCurrentResource);
		}
	}

	if (isInRenderingThread) {
		mCurrentResourceRT = mCurrentResource;
	} else {
		void * ResourceToRT = mCurrentResource;
		ExecuteOnRenderThread_DoNotWait([this, ResourceToRT](FRHICommandListImmediate& RHICmdList) {
			mCurrentResourceRT = ResourceToRT;
		});
	}
}

void* FWaveVRTextureManager::GetCurrentTextureResource()
{
	LOG_FUNC();
	return mCurrentResource;
}

/* unused */
WVR_Texture_t FWaveVRTextureManager::GetCurrentWVRTexture()
{
	LOG_FUNC();
	return (WVR_Texture_t) mCurrentResource;
}

/* unused */
WVR_TextureQueueHandle_t FWaveVRTextureManager::GetWVRTextureQueue() {
	LOG_FUNC();
	if (!mColorTexturePool) return nullptr;
	auto info = mColorTexturePool->GetInfo();
	return info.wvrTextureQueue;
}


WVR_TextureParams_t FWaveVRTextureManager::GetSubmitParams(WVR_Eye eye) {
	LOG_FUNC();
	WVR_TextureParams_t params = {0};
	if (!mColorTexturePool) return params;
	auto info = mColorTexturePool->GetInfo();

	float pixelDensity = mHMD->GetPixelDenity();
	float margin = (1.0f - pixelDensity) / 2; // TODO How to handle pixelDensity larger than 1?

	// Submit
	params.id = mCurrentResourceRT;
	if (wvrTextureTarget == WVR_TextureTarget_2D_DUAL)
	{
		params.target = wvrTextureTarget;
		float eyeOffset = eye == WVR_Eye_Left ? 0 : 0.5f;
		params.layout.leftLowUVs.v[0] = (0 + margin) / 2 + eyeOffset;
		params.layout.leftLowUVs.v[1] = (0 + margin);
		params.layout.rightUpUVs.v[0] = (1 - margin) / 2 + eyeOffset;
		params.layout.rightUpUVs.v[1] = (1 - margin);
	}
	else
	{
		params.target = wvrTextureTarget;
		params.layout.leftLowUVs.v[0] = (0 + margin);
		params.layout.leftLowUVs.v[1] = (0 + margin);
		params.layout.rightUpUVs.v[0] = (1 - margin);
		params.layout.rightUpUVs.v[1] = (1 - margin);
	}

	return params;
}

void FWaveVRTextureManager::CleanTextures()
{
	LOG_FUNC();
	if (mColorTexturePool != nullptr) {
		mColorTexturePool->ReleaseTextures();
		delete mColorTexturePool;
	}
	mColorTexturePool = nullptr;
}



/****************************************************
 *
 * FWaveVRRender
 *
 ****************************************************/

FWaveVRRender::FWaveVRRender(FWaveVRHMD * hmd) :
	FXRRenderTargetManager(),
	mCustomPresent(nullptr),
	mTextureManager(this, hmd),
	mHMD(hmd),

	// WaveVR State
	bInitialized(false),

	// WaveVR Settings
	msaaLevel(1),
	width(1440),
	height(1440),
	pixelFormat(PF_R8G8B8A8),
	isFoveatedRenderingEnabled(false),
	isMultiViewEnabled(false),
	defaultQueueSize(3),
	isSRGB(false),

	bSubmitWithPose(false),
	bSubmitWithInteralPose(false),

	// Unreal state
	needReAllocateRenderTargetTexture(false),
	needReAllocateDepthTexture(false)

{
	LOG_FUNC();

	if (PLATFORM_WINDOWS) {
		isMultiViewEnabled = false;

		useUnrealTextureQueue = WVRRENDER_USE_UNREAL_TEXTURE_QUEUE(true);
		createFromResource = WVRRENDER_CREATE_FROM_RESOURCE(false);
	} else if (PLATFORM_ANDROID) {
		msaaLevel = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.MobileMSAA"))->GetValueOnAnyThread();
		LOGD(WVRRender, "isMobileMultiViewEnabled %d, MobileMSAALevel %d", isMultiViewEnabled, msaaLevel);

		auto CVarMobileMultiView = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("vr.MobileMultiView"));
		isMultiViewEnabled = (CVarMobileMultiView && CVarMobileMultiView->GetValueOnAnyThread() != 0);

		// Enable MultiView
		GSupportsMobileMultiView = isMultiViewEnabled;

		// According to our experiment, which is noted above.  Different cases need different configurations.
		if (GSupportsMobileMultiView) {
			useUnrealTextureQueue = WVRRENDER_USE_UNREAL_TEXTURE_QUEUE(true);
			createFromResource = WVRRENDER_CREATE_FROM_RESOURCE(true);
		}
		else {
			useUnrealTextureQueue = WVRRENDER_USE_UNREAL_TEXTURE_QUEUE(true);
			createFromResource = WVRRENDER_CREATE_FROM_RESOURCE(false);
		}
	}
}

FWaveVRRender::~FWaveVRRender()
{
	LOG_FUNC();
}

void FWaveVRRender::RenderInit()
{
	LOG_FUNC();
	if (IsInRenderingThread())
	{
		RenderInit_RenderThread();
	}
	else
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(RenderInit,
			FWaveVRRender*, pRender, this,
			{
				pRender->RenderInit_RenderThread();
			});
		FlushRenderingCommands();
	}
}

void FWaveVRRender::UpdateConsoleVariable()
{
	// Console Variable
	auto CVarRMEnable = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("wvr.RenderMask.enable"));
	isRenderMaskEnabled = CVarRMEnable->GetValueOnAnyThread() != 0;

	//Init Eye Tracking param
	auto CVarETEnable = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("wvr.EyeTracking.enable"));
	isEyeTrackingEnabled = CVarETEnable->GetValueOnAnyThread() != 0;

	//Init Foveation Param
	auto CVarFREnable = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("wvr.FoveatedRendering.enable"));
	isFoveatedRenderingEnabled = CVarFREnable->GetValueOnAnyThread() != 0;

	auto CVarFRPeripheralFOV = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("wvr.FoveatedRendering.peripheralFOV"));
	int32 frPeripheralFOV = CVarFRPeripheralFOV->GetValueOnAnyThread();

	auto CVarFRPeripheralQuality = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("wvr.FoveatedRendering.peripheralQuality"));
	WVR_PeripheralQuality frPeripheralQuality = (WVR_PeripheralQuality)CVarFRPeripheralQuality->GetValueOnAnyThread();

	foveationParamsArray[0].focalX = foveationParamsArray[0].focalY = 0.0f;
	foveationParamsArray[0].fovealFov = frPeripheralFOV;
	foveationParamsArray[0].periQuality = frPeripheralQuality;
	foveationParamsArray[1].focalX = foveationParamsArray[1].focalY = 0.0f;
	foveationParamsArray[1].fovealFov = frPeripheralFOV;
	foveationParamsArray[1].periQuality = frPeripheralQuality;

	// DEBUG
	//LOGI(WVRRender, "FoveatedRendering: Left Eye focalX (%f) focalY (%f) FOV(%f) periQuality(%d)",
	//	foveationParamsArray[0].focalX, foveationParamsArray[0].focalY,
	//	foveationParamsArray[0].fovealFov, foveationParamsArray[0].periQuality);

	//LOGI(WVRRender, "FoveatedRendering: Right Eye focalX (%f) focalY (%f) FOV(%f) periQuality(%d)",
	//	foveationParamsArray[1].focalX, foveationParamsArray[1].focalY,
	//	foveationParamsArray[1].fovealFov, foveationParamsArray[1].periQuality);
}


void FWaveVRRender::RenderInit_RenderThread()
{
	LOG_FUNC();
	check(IsInRenderingThread());
	check(!IsInGameThread());

//#if WITH_EDITOR
//	if (GIsEditor) return;
//#endif

	UpdateConsoleVariable();

	{
		// We has the display config from device service, so we don't specify it.
		WVR_RenderInitParams_t param = {WVR_GraphicsApiType_OpenGL, uint64_t(WVR_RenderConfig_Default | isSRGB ? WVR_RenderConfig_sRGB : 0)};
		WVR_RenderError render_error = WVR_RenderError_LibNotSupported;

		render_error = WVR()->RenderInit(&param);
		if (render_error != WVR_RenderError_None) {
			ReportRenderError(render_error);
			LOGI(WVRRender,"WVR Render Init Failed");
			return;
		}
		else {
			LOGI(WVRRender, "WVR Render Init Success");
		}

		if (GIsGameThreadIdInitialized)
			WVR()->SetRenderThreadId(GGameThreadId);  // XXX ???????? GRenderThreadId

		{
			// Get default queue size.
			auto queue = WVR()->ObtainTextureQueue(GSupportsMobileMultiView ? WVR_TextureTarget_2D_ARRAY : WVR_TextureTarget_2D, WVR_TextureFormat_RGBA, WVR_TextureType_UnsignedByte, 256, 256, 0);
			defaultQueueSize = WVR()->GetTextureQueueLength(queue);
			WVR()->ReleaseTextureQueue(queue);

			check(defaultQueueSize);
		}
	}

	// Default
	WVR()->RenderFoveation(isFoveatedRenderingEnabled);

	WVR()->GetRenderTargetSize(&width, &height);
	LOGI(WVRRender, "WVR()->GetRenderTargetSize %u %u", width, height);

	// Initial TextureManager
	FWaveVRRenderTextureInfo info = {0};
	// common
	info.flags = TexCreate_RenderTargetable | TexCreate_ShaderResource | (isSRGB ? TexCreate_SRGB : 0);
	info.numSamples = msaaLevel;
	info.format = pixelFormat;  // PF_B8G8R8A8

	if (isMultiViewEnabled)
	{
		info.width = width;
		info.height = height;
		info.arraySize = 2;
		info.target = GL_TEXTURE_2D_ARRAY;
	}
	else
	{
		info.width = width * 2;
		info.height = height;
		info.arraySize = 1;
		info.target = /*msaaLevel > 1 ? GL_TEXTURE_2D_MULTISAMPLE :*/ GL_TEXTURE_2D; // MSAA need GL_TEXTURE_2D
	}

	info.useUnrealTextureQueue = useUnrealTextureQueue;
	info.createFromResource = createFromResource;

	info.wvrTextureQueue = nullptr;  // Created in CreateColorTexturePool.
	info.capacity = defaultQueueSize;

	mTextureManager.CreateColorTexturePool(info);

	bInitialized = true;
}


/** IStereoRenderTargetManager interface **/

bool FWaveVRRender::ShouldUseSeparateRenderTarget() const
{
	LOG_FUNC();
	return bInitialized;
}

void FWaveVRRender::UpdateViewport(bool bUseSeparateRenderTarget, const FViewport& Viewport, SViewport* ViewportWidget)
{
	LOG_FUNC();
	WVR_SCOPED_NAMED_EVENT(UpdateViewport, FColor::Purple);

//#if WITH_EDITOR
//	if (GIsEditor) goto end;
//#endif

	if (!bUseSeparateRenderTarget || !bInitialized) {
#if !WITH_EDITOR  // too noisy
		LOGW(WVRRender, "bUseSeparateRenderTarget=%d, bInitialized=%d",
			bUseSeparateRenderTarget, bInitialized);
#endif
		goto end;
	}

	if (needReAllocateRenderTargetTexture) {
		LOGD(WVRRender, "needReAllocateRenderTargetTexture, no next texture.");
		goto end;
	}

	// Get next texture
	mTextureManager.Next(Viewport);

end:
	// Must not remove this line:
	FXRRenderTargetManager::UpdateViewport(bUseSeparateRenderTarget, Viewport, ViewportWidget);
}

uint32 FWaveVRRender::GetNumberOfBufferedFrames() const {
	LOG_FUNC();
	return useUnrealTextureQueue ? defaultQueueSize : 1;
}

void FWaveVRRender::CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY)
{
	LOG_FUNC();
	if (PLATFORM_ANDROID) {
		InOutSizeX = GSupportsMobileMultiView ? GetSingleEyePixelWidth() : GetSingleEyePixelWidth() * 2;
		InOutSizeY = GetSingleEyePixelHeight();
	}
	else if (PLATFORM_WINDOWS || WITH_EDITOR) {
		InOutSizeX = GetSingleEyePixelWidth() * 2;
		InOutSizeY = GetSingleEyePixelHeight();
	}

	// This line call to HMD->GetIdealRenderTargetSize() then back here.  We can answer it directly.
	//FXRRenderTargetManager::CalculateRenderTargetSize(Viewport, InOutSizeX, InOutSizeY);
}

bool FWaveVRRender::NeedReAllocateViewportRenderTarget(const class FViewport& Viewport)
{
	LOG_FUNC();
//#if WITH_EDITOR
//	if (GIsEditor) return false;
//#endif

	//LOGV(WVRRender, "NeedReAllocateViewportRenderTarget return %d", needReAllocateRenderTargetTexture && bInitialized);

	// This line will call CalculateRenderTargetSize
	bool sizeMatch = FXRRenderTargetManager::NeedReAllocateViewportRenderTarget(Viewport);

	return (needReAllocateRenderTargetTexture && bInitialized) || sizeMatch;
}

bool FWaveVRRender::NeedReAllocateDepthTexture(const TRefCountPtr<struct IPooledRenderTarget>& DepthTarget)
{
	//LOG_FUNC();
//#if WITH_EDITOR
//	if (GIsEditor) return false;
//#endif

	//LOGD(WVRRender, "NeedReAllocateDepthTexture return %d", needReAllocateDepthTexture && bInitialized);
	return needReAllocateDepthTexture && bInitialized;
}

class FXRRenderBridge* FWaveVRRender::GetActiveRenderBridge_GameThread(bool bUseSeparateRenderTarget)
{
	LOG_FUNC();
//#if WITH_EDITOR
//	if (GIsEditor) return nullptr;
//#endif

	check(IsInGameThread());

	if (bUseSeparateRenderTarget)
	{
		bAlreadySetCustomPresent = true;
		if (mCustomPresent == nullptr)
			mCustomPresent = new FWaveVRFXRRenderBridge(this);
		return mCustomPresent;
	}
	else
	{
		bAlreadySetCustomPresent = false;
		return nullptr;
	}
}

bool FWaveVRRender::AllocateRenderTargetTexture(
	uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags,
	uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture,
	FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples)
{
	//LOG_FUNC();
	LOGI(WVRRender, "AllocateRenderTargetTexture TextureInfo width=%u height=%u mips=%u samples=%u flags=%u targetFlags=%u, format=%u",
		SizeX, SizeY, NumMips, NumSamples, Flags, TargetableTextureFlags, Format);

//#if WITH_EDITOR
//	if (GIsEditor) return false;
//#endif

	if (!bInitialized) {
		LOGW(WVRRender, "Not initialized");
		return false;
	}

	if (!needReAllocateRenderTargetTexture)
		LOGI(WVRRender, "Unreal asked AllocateRenderTargetTexture()");

	FWaveVRRenderTextureInfo info = {0};
	info.width = SizeX;
	info.height = SizeY;
	info.format = Format;
	info.mips = NumMips;
	info.flags = Flags;
	info.extraFlags = TargetableTextureFlags;
	info.numSamples = NumSamples;
	info.createFromResource = false;

	auto ret = mTextureManager.AllocateColorTexture(info, Index);
	if (ret) {
		LOGI(WVRRender, "Color Texture[%d] Allocated", Index);
		OutTargetableTexture = OutShaderResourceTexture = ret;
		needReAllocateRenderTargetTexture = false;
	}

	return ret;
}

// May be not necessary
bool FWaveVRRender::AllocateDepthTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples)
{
	//LOG_FUNC();
	LOGI(WVRRender, "AllocateDepthTexture TextureInfo width=%u height=%u mips=%u samples=%u flags=%u targetFlags=%u, format=%u",
		SizeX, SizeY, NumMips, NumSamples, Flags, TargetableTextureFlags, Format);

//#if WITH_EDITOR
//	if (GIsEditor) return false;
//#endif

	if (!bInitialized) {
		LOGW(WVRRender, "Not initialized");
		return false;
	}

	if (!needReAllocateRenderTargetTexture)
		LOGI(WVRRender, "Unreal asked AllocateDepthTexture()");

	FWaveVRRenderTextureInfo info = {0};
	info.width = SizeX;
	info.height = SizeY;
	info.format = Format;
	info.flags = Flags;
	info.extraFlags = TargetableTextureFlags;
	info.numSamples = NumSamples;
	info.mips = NumMips;
	info.createFromResource = false;

	auto ret = mTextureManager.AllocateDepthTexture(info, Index);

	if (ret) {
		LOGI(WVRRender, "Depth Texture[%d] Allocated", Index);
		OutTargetableTexture = OutShaderResourceTexture = ret;
		needReAllocateDepthTexture = false;
	}

	return ret;
}

/* FWaveVRRender Get and Set */

void FWaveVRRender::SetSingleEyePixelSize(uint32 w, uint32 h)
{
	LOG_FUNC();
	if (width != w || height != h)
		isDirty = true;

	check(w != 0 && h != 0)

	LOGI(WVRRender, "SetSingleEyePixelSize(%u, %u)", w, h);
	width = w;
	height = h;
}

void FWaveVRRender::SetTextureFormat(EPixelFormat format)
{
	LOG_FUNC();
	if (pixelFormat != format)
		isDirty = true;
	pixelFormat = format;
}

void FWaveVRRender::SetMultiSampleLevel(int InMSAALevel)
{
	LOG_FUNC();
	if (msaaLevel != InMSAALevel)
		isDirty = true;
	msaaLevel = InMSAALevel;
}

void FWaveVRRender::SetMultiView(bool enable)
{
	LOG_FUNC();
	if (isMultiViewEnabled != enable)
		isDirty = true;
	isMultiViewEnabled = enable;
}

void FWaveVRRender::SetFoveatedRendering(bool enable)
{
	LOG_FUNC();
	if (isFoveatedRenderingEnabled != enable) {
		isFoveatedRenderingEnabled = enable;
		WVR()->RenderFoveation(isFoveatedRenderingEnabled);
	}
}

void FWaveVRRender::SetFoveationParams(EStereoscopicPass Eye, const WVR_RenderFoveationParams_t& foveationParams)
{
	LOG_FUNC();
	if (!isFoveatedRenderingEnabled)
		LOGD(WVRRender, "SetFoveationParams but Foveation is disabled now!");

	if (Eye == EStereoscopicPass::eSSP_LEFT_EYE) {
		foveationParamsArray[0] = foveationParams;
	}
	else {
		foveationParamsArray[1] = foveationParams;
	}

	LOGI(WVRRender, "Set eye(%d) focalX (%f) focalY (%f) FOV(%f) periQuality(%d)",
		Eye, foveationParams.focalX, foveationParams.focalY,
		foveationParams.fovealFov, foveationParams.periQuality);

	LOGI(WVRRender, "Left Eye focalX (%f) focalY (%f) FOV(%f) periQuality(%d)",
		foveationParamsArray[0].focalX, foveationParamsArray[0].focalY,
		foveationParamsArray[0].fovealFov, foveationParamsArray[0].periQuality);

	LOGI(WVRRender, "Right Eye focalX (%f) focalY (%f) FOV(%f) periQuality(%d)",
		foveationParamsArray[1].focalX, foveationParamsArray[1].focalY,
		foveationParamsArray[1].fovealFov, foveationParamsArray[1].periQuality);
}


void FWaveVRRender::SetSubmitWithPose(bool enable, const WVR_PoseState_t * pose) {
	if (enable) {
		if (pose != nullptr) {
			bSubmitWithInteralPose = false;
			mPoseUsedOnSubmit = *pose;
		}
		else {
			bSubmitWithInteralPose = true;
		}
	}
	bSubmitWithPose = enable;
}

int FWaveVRRender::GetMultiSampleLevel() const {
	return msaaLevel;
}

bool FWaveVRRender::GetMultiView() const {
	return isMultiViewEnabled;
}

bool FWaveVRRender::GetFoveatedRendering() const {
	return isFoveatedRenderingEnabled;
}

void FWaveVRRender::GetSingleEyePixelSize(uint32 &w, uint32 &h) const {
	w = width;
	h = height;
}

uint32 FWaveVRRender::GetSingleEyePixelWidth() const {
	return width;
}

uint32 FWaveVRRender::GetSingleEyePixelHeight() const {
	return height;
}

EPixelFormat FWaveVRRender::GetTextureFormat() const {
	return pixelFormat;
}

void FWaveVRRender::Apply()
{
	LOG_FUNC();
	if (!isDirty)
		return;

	isDirty = false;

	// Wait NeedReAllocate
	needReAllocateRenderTargetTexture = true;
	//needReAllocateDepthTexture = true;
}

// Called by HMD
void FWaveVRRender::OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
	LOG_FUNC();

	// The command will send to RenderThread
	if (!bInitialized)
		RenderInit();

	// Should render RenderMask here

	WVR_TextureParams_t paramsL = mTextureManager.GetSubmitParams(WVR_Eye_Left);
	WVR()->PreRenderEye(WVR_Eye_Left, &paramsL, &(foveationParamsArray[0]));
	if (!isMultiViewEnabled)
	{
		WVR_TextureParams_t paramsR = mTextureManager.GetSubmitParams(WVR_Eye_Right);
		WVR()->PreRenderEye(WVR_Eye_Right, &paramsR, &(foveationParamsArray[1]));
	}
}

// Present
void FWaveVRRender::OnFinishRendering_RenderThread()
{
	LOG_FUNC();

	if (needReAllocateRenderTargetTexture) {
		LOGD(WVRRender, "needReAllocateRenderTargetTexture, not to submit.");
		return;
	}

	SubmitFrame_RenderThread();
}

// May be also invoked by WaveVRSplash.
void FWaveVRRender::SubmitFrame_RenderThread() {
	LOG_FUNC();
	WVR_SCOPED_NAMED_EVENT(SubmitFrame_RenderThread, FColor::Orange);
	// Submit

	//LOGV(WVRRender, "WVR_SubmitFrame(param.id=%p .target=%d)", params.id, params.target);

	auto posePtr = bSubmitWithPose ?
		(bSubmitWithInteralPose ? &(mHMD->FrameDataRT->poses.wvrPoses[0].pose) : &mPoseUsedOnSubmit) :
		nullptr;

	WVR_SubmitExtend submitExtendFlags = WVR_SubmitExtend_PartialTexture; // (WVR_SubmitExtend)(WVR_SubmitExtend_PartialTexture | WVR_SubmitExtend_SystemReserved1);
	if (isMultiViewEnabled) {
		WVR_TextureParams_t paramsL = mTextureManager.GetSubmitParams(WVR_Eye_Left);
		WVR()->SubmitFrame(WVR_Eye_Left, &paramsL, posePtr, submitExtendFlags);
	} else {
		WVR_TextureParams_t paramsL = mTextureManager.GetSubmitParams(WVR_Eye_Left);
		WVR_TextureParams_t paramsR = mTextureManager.GetSubmitParams(WVR_Eye_Right);
		WVR()->SubmitFrame(WVR_Eye_Left, &paramsL, posePtr, submitExtendFlags);
		WVR()->SubmitFrame(WVR_Eye_Right, &paramsR, posePtr, submitExtendFlags);
	}
}

void FWaveVRRender::OnResume()
{
	LOG_FUNC();
#if PLATFORM_ANDROID //For GNativeAndroidApp
	if (IsInRenderingThread()) {
		WVR()->SetATWActive(true, GNativeAndroidApp->window);
		WVR()->SetRenderThreadId(GGameThreadId);
	} else {
		ENQUEUE_UNIQUE_RENDER_COMMAND(OnResume,
			{
				WVR()->SetATWActive(true, GNativeAndroidApp->window);
				WVR()->SetRenderThreadId(GGameThreadId);
			});
	}
#endif
}

void FWaveVRRender::OnPause()
{
	LOG_FUNC();
	//needReAllocateRenderTargetTexture = true;

	if (IsInRenderingThread()) {
		WVR()->PauseATW();
		// No, we don't release textures when suspend.  This will cause a resume problem.
		//mTextureManager.CleanTextures();
	} else {
		ENQUEUE_UNIQUE_RENDER_COMMAND(Reset,
			{
				WVR()->PauseATW();
				// No, we don't release textures when suspend.  This will cause a resume problem.
				// mColorTexturePool will be null
				//mTextureManager.CleanTextures();
			});
	}
}

void FWaveVRRender::Shutdown()
{
	LOG_FUNC();
	bInitialized = false;
	needReAllocateRenderTargetTexture = true;

	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(Shutdown,
		FWaveVRRender*, pRender, this,
		{
			WVR()->PauseATW();
			// No, we don't release textures when shutdown.  TextureManager will handle it.
			//mTextureManager.CleanTextures();
		});
}
