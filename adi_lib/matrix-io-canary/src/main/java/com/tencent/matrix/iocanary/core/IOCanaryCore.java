/*
 * Tencent is pleased to support the open source community by making wechat-matrix available.
 * Copyright (C) 2018 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the BSD 3-Clause License (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.tencent.matrix.iocanary.core;

import android.util.Log;

import com.tencent.matrix.iocanary.config.IOConfig;

import java.util.List;

/**
 * @author liyongjie
 * Created by liyongjie on 2017/6/6.
 */

public class IOCanaryCore implements OnJniIssuePublishListener {
    private static final String TAG = "Matrix.IOCanaryCore";

    private final IOConfig mIOConfig;

    private volatile boolean mIsStart;

    public IOCanaryCore() {
        mIOConfig = new IOConfig();
    }

    public void start() {
        initDetectorsAndHookers(mIOConfig);
        mIsStart = true;
    }

    public boolean isStart() {
        return mIsStart;
    }

    public void stop() {
        mIsStart = false;
        IOCanaryJniBridge.uninstall();
    }

    private void initDetectorsAndHookers(IOConfig ioConfig) {
        assert ioConfig != null;

        if (ioConfig.isDetectFileIOInMainThread()
                || ioConfig.isDetectFileIOBufferTooSmall()
                || ioConfig.isDetectFileIORepeatReadSameFile()) {
            IOCanaryJniBridge.install(ioConfig, this);
        }

    }

    @Override
    public void onIssuePublish(List<IOIssue> issues) {
        for (IOIssue issue : issues) {
            Log.i("zkw", issue.toString());
        }
    }
}
