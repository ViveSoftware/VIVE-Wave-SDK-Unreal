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

import android.util.Log;
import com.htc.vr.sdk.VR;
import java.util.HashMap;
import android.os.Bundle;

/**
 * Created by devin_yu on 2018/9/12.
 */

public class OEMConfig {
	private static final String TAG = "OEMConfig";
	private static OEMConfig mInstance = null;
	private VR mVR = null;

	private VR.OnRuntimeEventListener mListener = new VR.OnRuntimeEventListener() {
		@Override
		public void onVrEnvironmentVerifyCompleted(boolean hasExternalDisplay) {
			Log.i(TAG, "onVrEnvironmentVerifyCompleted");
			mRuntimeStarted = true;
			updateOEMConfig();
		}
		@Override
		public void onConfigChanged(final Bundle data) {
			Log.i(TAG, "onConfigChanged");
			updateOEMConfig();
		}

		@Override
		public void onNativeReady() {
		}
	};

	public native void initNative();
	public native void ConfigChangedNative();

	private boolean mRuntimeStarted = false;
	private boolean configChanged = false;
	HashMap<String, String> configStrMap = new HashMap<String, String>();
	private String[] KeyArray = {
		"controller_property",
		"battery_indicator",
		"controller_singleBeam"
	};

	public OEMConfig() {
		Log.i(TAG, "constructor and initNative");
		initNative();
	}

	public static OEMConfig getInstance() {
		if (mInstance == null) {
			mInstance = new OEMConfig();
		}

		return mInstance;
	}

	public void setDecorator(VR tVR) {
		Log.i(TAG, "mVR is updated.");
		mVR = tVR;
		mVR.setRuntimeEventListener(mListener);
	}

	public String getJsonRawData() {
		return getJsonRawDataByKey("controller_property");
	}

	public String getBatteryIndicatorData() {
		return getJsonRawDataByKey("battery_indicator");
	}

	public String getSingleBeamData() {
		return getJsonRawDataByKey("controller_singleBeam");
	}

	public String getJsonRawDataByKey(final String key) {
		String ret = "";
		if (configStrMap.containsKey(key)) {
			ret = configStrMap.get(key);
		} else {
			String value = mVR.getConfigData(key);
			if (value == null) {
				Log.i(TAG, "OEMConfig key " + key + " not found");
			} else {
				ret = value;
			}
		}
		Log.i(TAG, "OEMConfig key: " + key + " value: " + ret);
		return ret;
	}

	public void updateOEMConfig() {
		if (!mRuntimeStarted) {
			Log.i(TAG, "updateOEMConfig, runtime is not ready yet.");
			return;
		}

		if (mVR == null) {
			Log.i(TAG, "updateOEMConfig, VR is null.");
			return;
		}

		String value = "";
		configChanged = false;

		for (int i=0; i<KeyArray.length; i++) {
			String key = KeyArray[i];

			value = mVR.getConfigData(key);

			Log.d(TAG, key + " = " + value);
			if (value != null) {
				if (configStrMap.containsKey(key)) {
					String mapValue = configStrMap.get(key);
					if (!value.equals(mapValue)) {
						Log.i(TAG, "OEMConfig key " + key + " is updated to " + value);
						configStrMap.remove(key);
						configStrMap.put(key, value);
						configChanged = true;
					}
				} else {
					Log.i(TAG, "OEMConfig key " + key + " with value " + value + " added");
					configStrMap.put(key, value);
					configChanged = true;
				}
			}
		}

		if (configChanged) {
			Log.i(TAG, "onConfigChanged, call native.");
			ConfigChangedNative();
		}
	}
}
