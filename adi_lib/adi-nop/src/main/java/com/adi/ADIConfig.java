package com.adi;

public class ADIConfig {

    /**
     * ADI 监控的事件类型
     */
    public enum Type {

        /**
         * 监控 Java 对象内存的分配和释放
         */
        OBJECT_ALLOC_AND_FREE,

        /**
         * 监控多线程锁竞争
         */
        THREAD_MONITOR_CONTEND;

        /**
         * 给指定 Type 增加新的监听事件。
         * <br><b>这是后门方法，使用前一定要充分了解自己在做什么！</b></br>
         *
         * @param event
         */
        public void addEvent(int event) {
        }

        Type() {
        }
    }


    private ADIConfig() {
    }

    public static class Builder {


        /**
         * 设置事件监控的采样，单位毫秒，默认值是 0.8 毫秒。
         * <br><b>支持事件类型：<br></b>OBJECT_ALLOC_AND_FREE
         *
         * @param ms
         * @return
         */
        public Builder setSampleInterval(float ms) {
            return this;
        }

        /**
         * 设置 ADI 监控的事件类型，参考 {@link Type}
         *
         * @param type
         * @return
         */
        public Builder setEventType(Type type) {
            return this;
        }

        /**
         * 设置获取调用栈的深度，默认 10。
         * <br><b>支持事件类型：<br></b>
         * OBJECT_ALLOC_AND_FREE、THREAD_MONITOR_CONTEND
         *
         * @param stackDepth
         */
        public Builder setStackDepth(int stackDepth) {
            return this;
        }

        public ADIConfig build() {
            return new ADIConfig();
        }
    }


}
