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

/**
 * Created on 04/23/2019.
 */

import android.content.Context;
import android.util.Log;
import android.content.res.AssetFileDescriptor;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import android.os.Environment;
import android.content.ContentProvider;
import android.net.Uri;
import android.database.Cursor;
import android.content.ContentValues;
import android.content.ContentResolver;

import com.epicgames.ue4.GameActivity;

public class SoftwareIpd {

	private static final String TAG = "Java_SoftwareIpd";
	private static SoftwareIpd mInstance = null;
	public native void initNative();

	private Context mContext;

	//public static GameActivity _activity;

	public void setContext(Context ctx) {
		mContext = ctx;
	}

	public SoftwareIpd() {
		Log.i(TAG, "constructor and initNative");
		initNative();
	}

	public static SoftwareIpd getInstance() {
	if (mInstance == null) {
		mInstance = new SoftwareIpd();
	}
		return mInstance;
	}


	String[] projection = new String[]{ "component",
						"enable",
						"value"};

	GameActivity ga = GameActivity.getInstance();
	ContentResolver mContentResolver = ga.getContentResolver();

	public String[] read_ipd() {

		String m_mComponentName  = "<read_ipd m_mComponentName is empty>";
		String m_isEnable        = "<m_isEnable is empty>";
		String m_value           = "<m_value is empty>";
		String m_cursor_state    = "<m_cursor_state is empty>";
		String m_cursor_moveToNext = "<m_cursor_moveToNext is empty>";

		Cursor cursor = mContentResolver.query(Uri.parse("content://com.htc.vr.irrelevantsetting/sw_ipd"),
									projection, null, null, null);

		if ( cursor == null ) {
			//Log.i(TAG, "cursor is null");
			m_cursor_state = "<cursor is null>";
		} else {

			m_cursor_state = "<cursor is not null>";

			while (cursor.moveToNext()) {
				m_cursor_moveToNext = "<m_cursor_moveTodNext is trigger>";
				m_mComponentName    = cursor.getString(cursor.getColumnIndex("component"));
				m_isEnable          = cursor.getString(cursor.getColumnIndex("enable"));
				m_value             = cursor.getString(cursor.getColumnIndex("value"));
			}
		}

		cursor.close();

		return new String[] {m_mComponentName, m_isEnable, m_value, "true", m_cursor_state, m_cursor_moveToNext};
	}

	public boolean write_ipd(String in_value, String in_isEnable) {

		String m_mComponentName = "<none name>";
		//String packageName = "<none package name>";
		Log.i(TAG, "read before m_mComponentName: %s: " + m_mComponentName + ", Oz");

		//read component packageName
		String uri          = "content://vr_data2/Auth";
		String[] projection = new String[]{"name", "value"};
		String selection    = new String("name = ?");
		String data         = "test";
		String[] selectionArgs = new String[] {"auth_hmd"};
		Cursor cursor       = mContentResolver.query(Uri.parse(uri), projection, selection, selectionArgs, null);

		String m_data = "";

		if (cursor == null) {
			Log.i(TAG, "cursor is null Oz");
			return false;
		} else {
			Log.i(TAG, "cursor not null Oz");
			while (cursor.moveToNext()) {
				Log.i(TAG, "cursor moveToNext Oz");
				data = cursor.getString(cursor.getColumnIndex("value"));
				//m_mComponentName = data;
				m_data = data;
				Log.i(TAG, "cursor moveToNext Oz data: " + data);
				//Log.i(TAG, "cursor moveToNext Oz data: " + m_mComponentName);
				Log.i(TAG, "cursor moveToNext Oz data: " + m_data);
			}
			//cursor.close();
		}

		ContentValues cv;
		cv = new ContentValues();
		cv.put("component", m_mComponentName);
		Log.i(TAG, "cursor data:  " + m_mComponentName + " Oz finish.");
		//String[] select = m_mComponentName.split("/");
		String[] select = m_data.split("/");
		m_mComponentName = m_data.replace("/", "_");

		int cnt = 0;
		for( String token:select) {
			Log.i(TAG, "cursor m_mComponentName: token: " + cnt + "." + token + " Oz finish.");
			cnt++;
		}
		Log.i(TAG, "cursor m_mComponentName: select[0]: " + select[0] + ", Oz finish.");

		String packageName  = select[0];
		Log.i(TAG, "cursor mComponentName : " + m_mComponentName + ", Oz finish.");
		Log.i(TAG, "cursor packageName: " + packageName + ", Oz finish.");
		Log.i(TAG, "cursor in_isEnable: " + in_isEnable + ", Oz finish.");
		Log.i(TAG, "cursor in_value: " + in_value + ", Oz finish.");
		Log.i(TAG, "cursor m_data: " + m_data + ", Oz finish.");

		cv.put("pkg_name", packageName);
		cv.put("enable", in_isEnable); // 1: enable, 0: disable
		cv.put("value", in_value);
		//check whether it exists by reading
		//if data is existed, update it
		mContentResolver.update(Uri.parse("content://com.htc.vr.irrelevantsetting/sw_ipd"),
				cv, new String("component=?"), new String[]{m_mComponentName});

		Log.i(TAG, "read after m_mComponentName: " + m_mComponentName + ", Oz");
		//if data is new, insert new value
		mContentResolver.insert(Uri.parse("content://com.htc.vr.irrelevantsetting/sw_ipd"), cv);

		return true;
	}
}
