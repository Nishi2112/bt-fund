/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>              /* Zephyr RTOSのカーネル機能（スレッド、タイマー、同期など）を使用するため */
#include <zephyr/logging/log.h>          /* ログ出力機能（LOG_INF、LOG_ERRなど）を使用するため */
#include <zephyr/bluetooth/bluetooth.h>    /* Bluetooth基本機能（初期化、有効化など）を使用するため */
#include <zephyr/bluetooth/gap.h>          /* GAP（Generic Access Profile）機能を使用するため */
#include <dk_buttons_and_leds.h>           /* Nordic開発キットのボタンとLED制御機能を使用するため */

/* STEP 2.1 - Declare the Company identifier (Company ID) */
#define COMPANY_ID_CODE 0x0059             /* Nordic Semiconductorの会社識別コード（Bluetooth SIGから割り当て） */

/* STEP 2.2 - Declare the structure for your custom data  */
typedef struct adv_mfg_data {
    uint16_t company_code;  /* 会社識別コード */
    uint16_t number_press;  /* Button 1の押下回数 */
} adv_mfg_data_type;

#define USER_BUTTON DK_BTN1_MSK            /* ユーザーボタンとして開発キットのボタン1を使用 */

/* STEP 1 - Create an LE Advertising Parameters variable */
static const struct bt_le_adv_param *adv_param =
    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_NONE,  /* オプションなし */
            32,  /* 最小間隔: 20ms (32*0.625ms) */
            32,  /* 最大間隔: 20.625ms (32*0.625ms) */
            NULL); /* 非指向性アドバタイジング */

/* STEP 2.3 - Define and initialize a variable of type adv_mfg_data_type */
static adv_mfg_data_type adv_mfg_data = { COMPANY_ID_CODE, 0x00 };  /* 製造者固有データの変数を初期化（会社コードと押下回数0） */

static unsigned char url_data[] = { 0x17, '/', '/', 'a', 'c', 'a', 'd', 'e', 'm',  /* スキャンレスポンスに含めるURL（0x17はhttpsプレフィックス） */
				    'y',  '.', 'n', 'o', 'r', 'd', 'i', 'c', 's',
				    'e',  'm', 'i', '.', 'c', 'o', 'm' };
LOG_MODULE_REGISTER(Lesson2_Exercise2, LOG_LEVEL_INF);  /* このモジュールのロギングを登録（INFOレベル） */

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME                    /* prj.confで定義されたデバイス名を使用 */
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)            /* デバイス名の長さ（NULL文字を除く） */

#define RUN_STATUS_LED DK_LED1                               /* ステータス表示用LEDとしてLED1を使用 */
#define RUN_LED_BLINK_INTERVAL 500                          /* LED点滅間隔を500ミリ秒（0.5秒）に設定 */

static const struct bt_data ad[] = {                         /* アドバタイジングデータの配列 */
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),    /* BLE専用デバイス（BR/EDR非対応）フラグ */
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),  /* 完全なデバイス名 */
	/* STEP 3 - Include the Manufacturer Specific Data in the advertising packet. */
	BT_DATA(BT_DATA_MANUFACTURER_DATA, 
        (unsigned char *)&adv_mfg_data, 
        sizeof(adv_mfg_data)),
};

static const struct bt_data sd[] = {                         /* スキャンレスポンスデータの配列 */
	BT_DATA(BT_DATA_URI, url_data, sizeof(url_data)),   /* URIデータを含める */
};
/* STEP 5 - Add the definition of callback function and update the advertising data dynamically */
static void button_changed(uint32_t button_state, uint32_t has_changed)  /* ボタン状態変化時のコールバック関数 */
{
    if (has_changed & button_state & USER_BUTTON) {        /* USER_BUTTONが押された場合 */
        adv_mfg_data.number_press += 1;                     /* ボタン押下回数をインクリメント */
        bt_le_adv_update_data(ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));  /* アドバタイジングデータを動的に更新 */
    }
}

/* STEP 4.1 - Define the initialization function of the buttons and setup interrupt.  */
static int init_button(void)                                /* ボタン初期化関数 */
{
    int err;                                                /* エラーコード格納用変数 */

    err = dk_buttons_init(button_changed);                  /* ボタンを初期化し、コールバック関数を登録 */
    if (err) {                                              /* エラーチェック */
        printk("Cannot init buttons (err: %d)\n", err);    /* エラー時はメッセージを出力 */
    }

    return err;                                             /* エラーコードを返す */
}

int main(void)                                              /* メイン関数 */
{
	int blink_status = 0;                               /* LED点滅状態を管理する変数 */
	int err;                                            /* エラーコード格納用変数 */

	LOG_INF("Starting Lesson 2 - Exercise 2 \n");      /* 開始メッセージをログ出力 */

	err = dk_leds_init();                               /* LED初期化 */
	if (err) {                                          /* エラーチェック */
		LOG_ERR("LEDs init failed (err %d)\n", err);   /* エラー時はログ出力 */
		return -1;                                  /* エラー終了 */
	}
	/* STEP 4.2 - Setup buttons on your board  */
	err = init_button();                                /* ボタン初期化関数を呼び出し */
	if (err) {                                          /* エラーチェック */
		printk("Button init failed (err %d)\n", err);  /* エラー時はメッセージ出力 */
		return -1;                                  /* エラー終了 */
}

	err = bt_enable(NULL);                              /* Bluetoothスタックを有効化（コールバックなし） */
	if (err) {                                          /* エラーチェック */
		LOG_ERR("Bluetooth init failed (err %d)\n", err);  /* エラー時はログ出力 */
		return -1;                                  /* エラー終了 */
	}

	LOG_INF("Bluetooth initialized\n");                /* 初期化成功をログ出力 */

	err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));  /* アドバタイジング開始 */
	if (err) {                                          /* エラーチェック */
		LOG_ERR("Advertising failed to start (err %d)\n", err);  /* エラー時はログ出力 */
		return -1;                                  /* エラー終了 */
	}

	LOG_INF("Advertising successfully started\n");     /* アドバタイジング開始成功をログ出力 */

	for (;;) {                                          /* 無限ループ */
		dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);  /* LED1を点滅（ON/OFF切り替え） */
		k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));    /* 1秒間スリープ */
	}
}
