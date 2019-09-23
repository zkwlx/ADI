package com.adi;

import java.util.Arrays;

import static com.adi.JVMTIConstant.*;

public class ADIConfig {

    /**
     * ADI 监控的事件类型
     */
    public enum Type {

        /**
         * 监控 Java 对象内存的分配和释放
         */
        OBJECT_ALLOC_AND_FREE(JVMTI_EVENT_VM_OBJECT_ALLOC, JVMTI_EVENT_OBJECT_FREE),

        /**
         * 监控多线程锁竞争
         */
        THREAD_MONITOR_CONTEND(JVMTI_EVENT_MONITOR_CONTENDED_ENTER, JVMTI_EVENT_MONITOR_CONTENDED_ENTERED),
        ;

        private int[] events;

        /**
         * 给指定 Type 增加新的监听事件。
         * <br><b>这是后门方法，使用前一定要充分了解自己在做什么！</b></br>
         *
         * @param event
         */
        public void addEvent(int event) {
            int newLength = events.length + 1;
            events = Arrays.copyOf(events, newLength);
            events[newLength - 1] = event;
        }

        Type(int... events) {
            this.events = events;
        }
    }

    private float sampleIntervalMs;

    private int[] events;

    public float getSampleIntervalMs() {
        return sampleIntervalMs;
    }

    public int[] getEvents() {
        return events;
    }

    private ADIConfig() {
    }

    public static class Builder {

        private float sampleIntervalMs = 0.8F;

        private int[] events;

        /**
         * 设置 Object Alloc 事件监控的采样，单位毫秒，默认值是 0.8 毫秒
         *
         * @param ms
         * @return
         */
        public Builder setSampleIntervalForObjectAlloc(float ms) {
            sampleIntervalMs = ms;
            return this;
        }

        /**
         * 设置 ADI 监控的事件类型，参考 {@link Type}
         *
         * @param type
         * @return
         */
        public Builder setEventType(Type type) {
            this.events = type.events;
            return this;
        }

        public ADIConfig build() {
            ADIConfig config = new ADIConfig();
            config.sampleIntervalMs = this.sampleIntervalMs;
            config.events = this.events;
            return config;
        }
    }


}
