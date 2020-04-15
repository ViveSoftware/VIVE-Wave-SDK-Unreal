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
import android.content.Intent;
import android.os.Handler;
import android.util.Log;

import java.lang.Runnable;

import com.htc.vr.sdk.VR;
import com.htc.vr.unreal.*;
import com.epicgames.ue4.GameActivity;

public class FadeOutStaticActivity {

    public static String TAG="FadeOutStaticActivityRunable";

    public static FadeOutStaticActivityRunable mFadeOutStaticActivityRunable;
    public static Handler mHandler;
    public static Context hContext;
    public static VR mVR;

    //private static VR mVR;

    public FadeOutStaticActivity() {
    }

    static class FadeOutStaticActivityRunable implements Runnable {
        public FadeOutStaticActivityRunable(){
            if ( hContext.getMainLooper() == null ) {
                Log.e(TAG, "hContext.getMainLooper() is null.");
            }
            mHandler = new Handler(hContext.getMainLooper());
            if ( mHandler == null ) {
                Log.e(TAG, "mHandler is null.");
            }
        };

        @Override
        public void run() {
                Runnable rable = new Runnable() {
                    @Override
                    public void run() {
                        Intent pIntent = new Intent(Intent.ACTION_MAIN);
                        if ( pIntent == null ) {
                            Log.e(TAG, "pIntent is null.");
                        }
                        pIntent.addCategory(Intent.CATEGORY_HOME);
                        pIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
                        pIntent.putExtra("QuickMenuEvent", 1);
                        hContext.startActivity(pIntent);
                    }
                };
            Log.d(TAG, "mVR.doVRStartActivity begin.");
            mVR.doVRStartActivity(rable);
            Log.d(TAG, "mVR.doVRStartActivity finish.");
        }
    }

    public static void onFadeOutStaticActivity(Context context, VR vr) {
        Log.d(TAG, "onFadeOutStaticActivity begin.");
        hContext = context;
        mVR = vr;
        mFadeOutStaticActivityRunable = new FadeOutStaticActivityRunable();
        if ( mFadeOutStaticActivityRunable == null ) {
            Log.e(TAG, "mFadeOutStaticActivityRunable is null.");
        }
        mHandler.postDelayed(mFadeOutStaticActivityRunable, 1);
        Log.d(TAG, "onFadeOutStaticActivity finish.");
    } //onFadeOutStaticActivity();
}
