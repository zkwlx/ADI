package com.adi;

public class ADIConfig {

    private int sampleIntervalMs;

    public int getSampleIntervalMs() {
        return sampleIntervalMs;
    }

    private ADIConfig() {
    }

    public static class Builder {

        private int sampleIntervalMs = 0;

        public Builder setSampleIntervalMs(int ms) {
            sampleIntervalMs = ms;
            return this;
        }

        public ADIConfig build() {
            ADIConfig config = new ADIConfig();
            config.sampleIntervalMs = this.sampleIntervalMs;
            return config;
        }
    }


}
