package com.adi;

public class ADIConfig {

    private float sampleIntervalMs;

    public float getSampleIntervalMs() {
        return sampleIntervalMs;
    }

    private ADIConfig() {
    }

    public static class Builder {

        private float sampleIntervalMs = 0;

        public Builder setSampleIntervalMs(float ms) {
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
