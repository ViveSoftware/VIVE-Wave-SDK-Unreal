// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#pragma once

#include "UnrealClient.h"
#include "XRRenderTargetManager.h"

#include "OpenGLDrv.h"
#include "XRRenderBridge.h"

class FWaveVRHMD;
class FWaveVRRender;
class FWaveVRTexturePool;

class FWaveVRFXRRenderBridge : public FXRRenderBridge
{
public:
	FWaveVRFXRRenderBridge(FWaveVRRender * render);
	virtual ~FWaveVRFXRRenderBridge();

	// FRHICustomPresent
	virtual bool NeedsNativePresent() override; //D3D12 only now, not mobile.
	virtual bool Present(int& SyncInterval) override;
	virtual void OnBackBufferResize() override;

#if 0
	// Called from RHI thread after native Present has been called
	virtual void PostPresent() override {};
	// Called when rendering thread is acquired
	virtual void OnAcquireThreadOwnership() override {}
	// Called when rendering thread is released
	virtual void OnReleaseThreadOwnership() override {}
#endif

private:
	FWaveVRFXRRenderBridge(const FWaveVRFXRRenderBridge &) = delete;
	FWaveVRFXRRenderBridge(FWaveVRFXRRenderBridge &&) = delete;
	FWaveVRFXRRenderBridge &operator=(const FWaveVRFXRRenderBridge &) = delete;

private:
	FWaveVRRender * mRender;
};

struct FWaveVRRenderTextureInfo
{
	uint32 width;
	uint32 height;
	uint32 arraySize;  // Multiview layer
	uint32 flags;
	uint32 extraFlags;
	uint8 mips;
	uint8 format;
	uint8 numSamples;
	uint8 capacity;
	GLenum target;

	bool createFromResource;  // If true, use the texture in wvrTextureQueue
	bool useUnrealTextureQueue;  // If true, we don't need create a base texture
	bool useWVRTextureQueue;  // Decided by TextureManager
	WVR_TextureQueueHandle_t wvrTextureQueue;  // Created by TextureManager
};

// Manager queue and submit
class FWaveVRTextureManager
{
private:
	friend class FWaveVRFXRRenderBridge;

public:
	FWaveVRTextureManager(FWaveVRRender * render, FWaveVRHMD * hmd);
	~FWaveVRTextureManager();

public:
	bool CreateColorTexturePool(const FWaveVRRenderTextureInfo& info);
	//FTexture2DRHIRef CreateDepthTexture(const FWaveVRRenderTextureInfo& info);

	// Get next texture from pool
	void Next(const FViewport& Viewport);

	void CleanTextures();

public:
	FTexture2DRHIRef AllocateColorTexture(const FWaveVRRenderTextureInfo& engineInfo, uint32 index);
	FTexture2DRHIRef AllocateDepthTexture(const FWaveVRRenderTextureInfo& engineInfo, uint32 index);

	WVR_Texture_t GetCurrentWVRTexture();
	void * GetCurrentTextureResource();
	WVR_TextureQueueHandle_t GetWVRTextureQueue();
	WVR_TextureParams_t GetSubmitParams(WVR_Eye eye);

//private:
//	inline FWaveVRTexturePool * GetColorTextureQueue() const { return mColorTexturePool; }
//	inline FWaveVRTexturePool * GetDepthTextureQueue() const { return mDepthTextureQueue; }

private:
	FWaveVRTextureManager(const FWaveVRTextureManager &) = delete;
	FWaveVRTextureManager(FWaveVRTextureManager &&) = delete;
	FWaveVRTextureManager &operator=(const FWaveVRTextureManager &) = delete;

private:
	FWaveVRTexturePool * mColorTexturePool;

	void * mCurrentResource;
	void * mCurrentResourceRT;
	uint32 mCurrentWVRTextureQueueIndex;  // Sometimes the wvr texture queue will return -1.  The useUnrealTextureQueue help handle it.

	WVR_TextureTarget wvrTextureTarget;

protected:
	FWaveVRRender * mRender;
	FWaveVRHMD * mHMD;
};

// Manager queue and submit
class FWaveVRRender : public FXRRenderTargetManager
{
public:
	FWaveVRRender(FWaveVRHMD * hmd);
	virtual ~FWaveVRRender();

private:
	FWaveVRRender(const FWaveVRRender &) = delete;
	FWaveVRRender(FWaveVRRender &&) = delete;
	FWaveVRRender &operator=(const FWaveVRRender &) = delete;

public:
	inline FWaveVRTextureManager * GetTextureManager() { return &mTextureManager; }
	inline bool IsInitialized() const { return bInitialized; }
	inline bool IsCustomPresentSet() const { return bAlreadySetCustomPresent; }

public:
	// Set
	void SetMultiSampleLevel(int msaaLevel);
	void SetMultiView(bool enable);
	void SetFoveatedRendering(bool enable);
	void SetSingleEyePixelSize(uint32 w, uint32 h);
	void SetTextureFormat(EPixelFormat format);
	void SetFoveationParams(EStereoscopicPass Eye, const WVR_RenderFoveationParams_t& FoveatParams);
	// If pose is nullptr, use internal pose.  Only benifted when late update is enabled.
	void SetSubmitWithPose(bool enable, const WVR_PoseState_t * pose = nullptr);

	// Get
	int GetMultiSampleLevel() const;
	bool GetMultiView() const;
	bool GetFoveatedRendering() const;
	void GetSingleEyePixelSize(uint32 &w, uint32 &h) const;
	uint32 GetSingleEyePixelWidth() const;
	uint32 GetSingleEyePixelHeight() const;
	EPixelFormat GetTextureFormat() const;

	void Apply();

public:
	void RenderInit();
	void RenderInit_RenderThread();
	void UpdateConsoleVariable();

	// Called by HMD
	void OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily);
	// Called by custom present
	void OnFinishRendering_RenderThread();

	void SubmitFrame_RenderThread();

	// Called by HMD
	void OnResume();
	void OnPause();
	void Shutdown();


/** IStereoRenderTargetManager interface **/
public:
	/**
	 * Whether a separate render target should be used or not.
	 * In case the stereo rendering implementation does not require special handling of separate render targets
	 * at all, it can leave out implementing this interface completely and simply let the default implementation
	 * of IStereoRendering::GetRenderTargetManager() return nullptr.
	 */
	virtual bool ShouldUseSeparateRenderTarget() const override;

	/**
	* Returns true, if render target texture must be re-calculated.
	*/
	virtual bool NeedReAllocateDepthTexture(const TRefCountPtr<struct IPooledRenderTarget>& DepthTarget) override;

	/**
		* Returns number of required buffered frames.
		*/
	virtual uint32 GetNumberOfBufferedFrames() const override;

	/**
	 * Allocates a render target texture.
	 * The default implementation always return false to indicate that the default texture allocation should be used instead.
	 *
	 * @param Index			(in) index of the buffer, changing from 0 to GetNumberOfBufferedFrames()
	 * @return				true, if texture was allocated; false, if the default texture allocation should be used.
	 */
	virtual bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override;

	/**
	 * Allocates a depth texture.
	 *
	 * @param Index			(in) index of the buffer, changing from 0 to GetNumberOfBufferedFrames()
	 * @return				true, if texture was allocated; false, if the default texture allocation should be used.
	 */
	virtual bool AllocateDepthTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override;

/** XRRenderTargetManager interface **/
public:
	/**
	 * Updates viewport for direct rendering of distortion. Should be called on a game thread.
	 *
	 * @param bUseSeparateRenderTarget	Set to true if a separate render target will be used. Can potentiallt be true even if ShouldUseSeparateRenderTarget() returned false earlier.
	 * @param Viewport					The Viewport instance calling this method.
	 * @param ViewportWidget			(optional) The Viewport widget containing the view. Can be used to access SWindow object.
	 */
	virtual void UpdateViewport(bool bUseSeparateRenderTarget, const FViewport& Viewport, SViewport* ViewportWidget = nullptr) override;

	/**
	 * Calculates dimensions of the render target texture for direct rendering of distortion.
	 * This implementation calculates the size based on the current value of vr.pixeldensity.
	 */
	virtual void CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY) override;

	/**
	* Returns true, if render target texture must be re-calculated.
	*/
	virtual bool NeedReAllocateViewportRenderTarget(const class FViewport& Viewport) override;

protected:
	/**
	 * Return the current render bridge for use by the current viewport. Override this method if your device implements a custom FXRRenderBridge.
	 * Should only return non-null if the render bridge is initialized and should be used for the current frame
	 */
	virtual class FXRRenderBridge* GetActiveRenderBridge_GameThread(bool bUseSeparateRenderTarget) override;

private:
	TRefCountPtr<FWaveVRFXRRenderBridge> mCustomPresent;
	FWaveVRTextureManager mTextureManager;
	FWaveVRHMD * mHMD;

private:
	// WaveVR state
	bool bInitialized;
	bool bAlreadySetCustomPresent;

private:
	// WaveVR settings
	int msaaLevel;
	uint32 width, height;
	EPixelFormat pixelFormat;
	bool isFoveatedRenderingEnabled;
	bool isRenderMaskEnabled;
	bool isEyeTrackingEnabled;
	bool isMultiViewEnabled;
	bool isDirty;
	uint32 defaultQueueSize;
	bool isSRGB;

private:
	// Pose
	WVR_PoseState_t mPoseUsedOnSubmit;

private:
	bool bSubmitWithPose;
	bool bSubmitWithInteralPose;

private:
	WVR_RenderFoveationParams_t foveationParamsArray[2];

private:
	bool needReAllocateRenderTargetTexture;
	bool needReAllocateDepthTexture;
	bool useUnrealTextureQueue;
	bool createFromResource;
};
