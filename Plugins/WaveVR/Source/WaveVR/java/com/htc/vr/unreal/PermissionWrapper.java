// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

package com.htc.vr.unreal;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Build;
import android.util.Log;

import java.util.Locale;
import com.htc.vr.permission.client.PermissionCallback;
import com.htc.vr.permission.client.UsbPermissionCallback;
import com.htc.vr.permission.client.GrantedResult;
import com.htc.vr.permission.client.UsbGrantedResult;
import com.htc.vr.permission.client.PermissionManager;
import com.epicgames.ue4.GameActivity;
/**
 * Created by on 2018/7/5.
 */

public class PermissionWrapper {
	private static final String TAG = "PermissionWrapper";
	private static PermissionWrapper mInstance = null;
	public native void initNative();
	public native void requestCallbackNative(String[] requestNames, boolean[] result);
	public native void requestUsbCallbackNative(boolean result);

	public static PermissionWrapper getInstance() {
		if (mInstance == null) {
			mInstance = new PermissionWrapper();
		}

		return mInstance;
	}

	public PermissionWrapper() {
		Log.i(TAG, "constructor and initNative");
		initNative();
	}

	public PermissionCallback mCB = new PermissionCallback() {
		public void onRequestCompletedwithObject(GrantedResult gs) {
			//int strLen = gs.requestPermissions.length;
			//int resLen = gs.result.length;
			Log.d(TAG, "PermissionCallback");
			requestCallbackNative(gs.requestPermissions, gs.result);
		}
	};

	public UsbPermissionCallback mUsbCB = new UsbPermissionCallback() {
		public void onRequestCompletedwithObject(UsbGrantedResult gs) {
			//int strLen = gs.requestPermissions.length;
			//int resLen = gs.result.length;
			Log.d(TAG, "PermissionCallback");
			requestUsbCallbackNative(gs.result);
		}
	};




	public boolean requestPermissions(String[] requestPermissions) {
		Log.d(TAG, "requestPermissions");
		PermissionManager mPM = PermissionManager.getInstance();

		if (!mPM.isInitialized()) return false;

		if(mPM.isShow2D()){
			GameActivity.getInstance().setRequestPermissionCallback(mCB);
		}
		mPM.requestPermissions(requestPermissions, mCB);
		return true;
	}

	public boolean requestUsbPermission() {
		Log.d(TAG, "requestUsbPermission");
		PermissionManager mPM = PermissionManager.getInstance();

		if (!mPM.isInitialized()) return false;

		mPM.requestUsbPermission(mUsbCB);
		return true;
	}



	public boolean isPermissionGranted(String permission) {
		Log.d(TAG, "isPermissionGranted");
		PermissionManager mPM = PermissionManager.getInstance();
		if (!mPM.isInitialized()) return false;
		return mPM.isPermissionGranted(permission);
	}

	public boolean shouldGrantPermission(String permission) {
		Log.d(TAG, "shouldGrantPermission");
		PermissionManager mPM = PermissionManager.getInstance();
		if (!mPM.isInitialized()) return false;
		return mPM.shouldGrantPermission(permission);
	}

	public boolean showDialogOnVRScene() {
		Log.d(TAG, "showDialogOnVRScene");
		PermissionManager mPM = PermissionManager.getInstance();
		if (!mPM.isInitialized()) return false;
		return mPM.showDialogOnVRScene();
	}
}
