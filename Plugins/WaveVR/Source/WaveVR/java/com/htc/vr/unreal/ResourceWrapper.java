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

/**
 * Created by on 2018/4/30.
 */

public class ResourceWrapper {
	private static final String TAG = "ResourceWrapper";
	private static ResourceWrapper mInstance = null;
	private static Context mContext = null;
	private static String mPackageName;

	public native void initNative();

	public void setPackageName(String pn) {
		mPackageName = pn;
		Log.i(TAG, "Package name = " + mPackageName);
	}

	public ResourceWrapper() {
		Log.i(TAG, "constructor and initNative");
		initNative();
	}

	public static ResourceWrapper getInstance() {
		if (mInstance == null) {
			mInstance = new ResourceWrapper();
		}

		return mInstance;
	}

	public void setContext(Context ctx) {
		mContext = ctx;
	}

	public String  getPreferredStringByName(String aString, String lang, String country) {
		Log.i(TAG, "getPreferredStringByName string = " + aString + " language = " + lang + " country = " + country);
		int resId = mContext.getResources().getIdentifier(aString, "string", mPackageName);

		if (resId == 0) {
			Log.i(TAG, "string is not found, return null string = " + aString);
			return "";
		}

		Configuration configuration = new Configuration(mContext.getResources().getConfiguration());
		configuration.setLocale(new Locale(lang, country));
		return mContext.createConfigurationContext(configuration).getResources().getString(resId);
	}

	public String getStringByName(String aString) {
		Log.i(TAG, "getStringByName string = " + aString);

		int resId = mContext.getResources().getIdentifier(aString, "string", mPackageName);

		if (resId == 0) {
			Log.i(TAG, "string is not found, return null string = " + aString);
			return "";
		}

		return mContext.getString(resId);
	}

	public String getCurrentLocale() {
		Locale current;
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
			current = mContext.getResources().getConfiguration().getLocales().get(0);
		} else {
			current = mContext.getResources().getConfiguration().locale;
		}

		Log.i(TAG, "current locale getDisplayLanguage = " + current.getDisplayLanguage());
		Log.i(TAG, "current locale getDisplayName = " + current.getDisplayName());
		Log.i(TAG, "current locale current.toString = " + current.toString());
		Log.i(TAG, "current locale current.getLanguage() = " + current.getLanguage());

		return current.toString();
	}

	public String getSystemLanguage() {
		Locale current;
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
			current = mContext.getResources().getConfiguration().getLocales().get(0);
		} else {
			current = mContext.getResources().getConfiguration().locale;
		}
		return current.getLanguage();
	}

	public String getSystemCountry() {
		Locale current;
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
			current = mContext.getResources().getConfiguration().getLocales().get(0);
		} else {
			current = mContext.getResources().getConfiguration().locale;
		}
		return current.getCountry();
	}
}
