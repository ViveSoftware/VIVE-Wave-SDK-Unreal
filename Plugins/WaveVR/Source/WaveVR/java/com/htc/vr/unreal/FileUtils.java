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
 * Created on 6/27/18.
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

import com.epicgames.ue4.GameActivity;

public class FileUtils {
	private static final String TAG = "Java_FileUtils";
	private Context mContext;
	private AssetFileDescriptor mAssetFileDescriptor;
	private static FileUtils mInstance = null;
	public native void initNative();

	public FileUtils() {
		Log.i(TAG, "constructor and initNative");
		initNative();
	}

	public FileUtils(Context context, AssetFileDescriptor afd) {
		Log.i(TAG, "constructor");
		mContext = context;
		mAssetFileDescriptor = afd;
	}

	public static FileUtils getInstance() {
		if (mInstance == null) {
			mInstance = new FileUtils();
		}

		return mInstance;
	}

	public void setContext(Context ctx) {
		mContext = ctx;
	}

	public String doUnZIPAndDeploy(String folder, int deviceIndex) {
		Log.i(TAG, "folder -> " + folder + " deviceIndex - > " + deviceIndex);

		String filename;
		String rootFolder = mContext.getFilesDir() + "/RenderModels/";
		String renderModelFolder = rootFolder + folder;
		Log.i(TAG, "renderModel folder -> " + renderModelFolder);
		File dir = new File(renderModelFolder);
		if (!dir.exists()) {
			if (mContext == null) {
				Log.e(TAG, "mContext is null");
				return "";
			}

			mAssetFileDescriptor = GameActivity.getInstance().getControllerModelFileDescriptor(deviceIndex);
			ZipInputStream zipTest;
			try {
				zipTest = new ZipInputStream(mAssetFileDescriptor.createInputStream());
			} catch (IOException e) {
				e.printStackTrace();
				Log.e(TAG, "can't load ZIP file");
				return "";
			}
			dir = new File(rootFolder);
			if (!dir.exists()) {
				dir.mkdirs();
			}

			ZipEntry mZipEntry;
			byte[] buffer = new byte[1024];
			int count;

			dir = new File(renderModelFolder);
			if (!dir.exists()) {
				dir.mkdirs();
			} else {
				if (dir.isDirectory()) {
					String[] children = dir.list();
					for (int i = 0; i < children.length; i++)
					{
					   new File(dir, children[i]).delete();
					}
				} else {
					dir.delete();
				}
			}

			try {
				while ((mZipEntry = zipTest.getNextEntry()) != null) {
					filename = mZipEntry.getName();

                    File verifyFile = new File(renderModelFolder + filename);

                    String absolutePath = verifyFile.getAbsolutePath();

                    if (!absolutePath.startsWith(renderModelFolder)) {
                        Log.w(TAG, "file name = " + filename + " illegal, absolute path = " + absolutePath);
                        continue;
                    }

					// Need to create directories if not exists, or
					// it will generate an Exception...
					if (mZipEntry.isDirectory()) {
						File fmd = new File(renderModelFolder + filename);
						fmd.mkdirs();
						Log.i(TAG, "mkdir -> " + renderModelFolder + filename);
						continue;
					}

					FileOutputStream fout = new FileOutputStream(renderModelFolder + filename);
					Log.i(TAG, "output -> " + renderModelFolder + filename);
					while ((count = zipTest.read(buffer)) != -1) {
						fout.write(buffer, 0, count);
					}
					fout.close();
					zipTest.closeEntry();
				}
			} catch (IOException e) {
				Log.e(TAG, "uncompressed ZIP file fails");
				e.printStackTrace();
				return "";
			}
		}
		String ModelStr = renderModelFolder + "Model/";
		File ModelFolder= new File(ModelStr);
		String UnrealStr = renderModelFolder + "Unreal/";
		File UnrealFolder = new File(UnrealStr);

		String retStr = "";
		if(ModelFolder.exists()) {
			retStr = ModelStr;
		} else if (UnrealFolder.exists()) {
			retStr = UnrealStr;
		} else {
			Log.e(TAG, "FBX file not found");
		}

		//String retStr = renderModelFolder + "Unreal/";

		Log.i(TAG, "output -> " + retStr);

		return retStr;
	}
}
