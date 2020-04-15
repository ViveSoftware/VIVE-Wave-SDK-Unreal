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

/***
 * Created by BabelSW.Hsu on 2018-6-13
 **/
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.net.Uri;

import android.util.Log;

public class ContentProvider {
	private static String LOG_TAG = "WaveVR_ContentProvider_java";
	private static Context mContext = null;
	private static ContentProvider mInstance = null;
	private ContentResolver resolver = null;

	/// Controller default role.
	private static String name_Role = "role_default";
	private static String uriString_Role = "content://vr_data2/Role";
	private Uri uri_Role = Uri.parse(uriString_Role);

	public native void initNative();

	public ContentProvider() {
		Log.i(LOG_TAG, "ContentProvider() and initNative()");
		initNative();
	}

	public static ContentProvider getInstance() {
		Log.i(LOG_TAG, "getInstance()");
		if (mInstance == null) {
			mInstance = new ContentProvider();
		}

		return mInstance;
	}

	public void setContext(Context ctx) {
		Log.i(LOG_TAG, "setContext()");
		mContext = ctx;
		resolver = mContext.getContentResolver();
	}

	public void writeControllerRoleValue(String value) {
		String selectionClause = new String("name = ?");
		String[] selectionArgs = new String [] {name_Role};
		ContentValues values = new ContentValues();
		values.put("name", name_Role);
		values.put("value", value);
		try {
			resolver.update(uri_Role, values, selectionClause, selectionArgs);
			Log.i(LOG_TAG, "writeControllerRoleValue() " + value);
		} catch (SecurityException e) {
			e.printStackTrace();
			Log.e(LOG_TAG, "SecurityException: writing com.htc.vr.core.server.VRDataProvider2 uri content://vr_data2/Role");
		}
	}
}
