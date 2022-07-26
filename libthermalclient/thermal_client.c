/*
 * Copyright (C) 2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <thermal_client.h>

int thermal_client_config_query(char *algo_type, struct config_instance **configs) {
    return 0;
}
void thermal_client_config_cleanup(struct config_instance *configs, unsigned int config_size) {
    return;
}
int thermal_client_config_set(struct config_instance *configs, unsigned int config_size) {
    return 0;
}

int thermal_client_register_callback(char *client_name, int (*callback)(int , void *, void *), void *data) {
    return 0;
}
int thermal_client_request(char *client_name, int req_data) {
    return 0;
}
void thermal_client_unregister_callback(int client_cb_handle) {
    return;
}

/* APIs for bandwidth clients to send/clear bandwidth perf levels to thermal-engine */
int thermal_bandwidth_client_request(char *client_name, int req_data) {
    return 0;
}
void thermal_bandwidth_client_cancel_request(char *client_name) {
    return;
}
