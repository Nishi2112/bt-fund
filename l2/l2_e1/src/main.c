/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
/* STEP 3 - Include the header file of the Bluetooth LE stack */

#include <dk_buttons_and_leds.h>

/***** exercise */
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
/* exercise *****/

LOG_MODULE_REGISTER(Lesson2_Exercise1, LOG_LEVEL_INF);

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define RUN_STATUS_LED DK_LED1
#define RUN_LED_BLINK_INTERVAL 1000

/* STEP 4.1.1 - Declare the advertising packet */

/* STEP 4.2.2 - Declare the URL data to include in the scan response */

/* STEP 4.2.1 - Declare the scan response packet */

int main(void)
{
	int blink_status = 0;
	int err;

	LOG_INF("Starting Lesson 2 - Exercise 1 \n");

	err = dk_leds_init();
	if (err) {
		LOG_ERR("LEDs init failed (err %d)\n", err);
		return -1;
	}
	/* STEP 5 - Enable the Bluetooth LE stack */

	/* STEP 6 - Start advertising */

	LOG_INF("Advertising successfully started\n");

	for (;;) {
		dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);
		k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));
	}
}

/***** exercise */
// アドバタイジングパケット (ad[]) の準備
static const struct bt_data ad[] = {
    /* STEP 4.1.2 - アドバタイジングフラグを設定 */
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),

    /* STEP 4.1.3 - アドバタイジングパケットデータを設定 */
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

// 4.2.2 送信するURLデータを定義
// 0x17はBluetoothの規格で定められた「https://」を意味する短縮コード。データ量を節約できる。
static unsigned char url_data[] = {0x17, '/', '/', 'a', 'c', 'a', 'd', 'e', 'm', 'y', '.',
                                   'n', 'o', 'r', 'd', 'i', 'c', 's', 'e', 'm', 'i', '.',
                                   'c', 'o', 'm'};

// 4.2.1 スキャンレスポンスパケットを宣言
static const struct bt_data sd[] = {
    // 4.2.3 URLデータをパケットに含める
    BT_DATA(BT_DATA_URI, url_data, sizeof(url_data)),
};

int err; // エラーコード格納用

// Bluetoothの初期化
err = bt_enable(NULL);
if (err) {
    LOG_ERR("Bluetooth init failed (err %d)\n", err);
    return -1;
}
LOG_INF("Bluetooth initialized\n");

// アドバタイジングを開始
err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
if (err) {
    LOG_ERR("Advertising failed to start (err %d)\n", err);
    return -1;
}
/* exercise *****/