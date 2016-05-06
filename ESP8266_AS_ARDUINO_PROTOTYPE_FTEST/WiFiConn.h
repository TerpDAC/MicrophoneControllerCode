#ifndef _WIFI_CONN_H
#define _WIFI_CONN_H
void connectToWiFi();
int getWiFiEnabled();
void wifiDisable();
void wifiEnable();
void wifiTest();
void espDeepSleep(uint32_t sec);
void espSleep(uint32_t sec);
void submitSum(uint32_t curTime, long totalSampleCount, long highTotalCount, long midTotalCount);
#endif
