#ifndef ENDPOINTS_H
#define ENDPOINTS_H

void handleMediciones();
void handleConfiguracion();
void habldePostConfig();
void handleData();
void handleSCD30Calibration();
void handleSettings();
void handleRestart();
void handleConfigReset();
void handleHome();

#ifdef ENABLE_ESPNOW
void handleESPNowStatus();
#endif

#endif // ENDPOINTS_H