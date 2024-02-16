[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver SFA30

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/sfa30/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

SFA30是一款基於電化學傳感技術的甲醛檢測模塊，針對最低的誤報率進行了優化。 SFA30 HCHO感測器對乙醇具有低交叉靈敏度，並經過優化，可在低百萬分之一（ppb）範圍內工作，以準確檢測世界衛生組織室內暴露參攷值周圍的低HCHO濃度。 通過集成Sensirion RHT感測器和集成微處理器進行溫度和濕度補償，確保在不斷變化的室內環境中具有可靠的效能。 該感測器能够承受儲存、搬運和運輸過程中的不利環境條件。 由於其先進的防幹科技，該感測器的使用壽命為6年。 每個SFA30都經過工廠校準，可以通過I2C或UART介面輕鬆集成到您的設備中。 低功耗允許電池運行。 這使得SFA30成為一種可靠、易於使用和高品質的HCHO和RHT組合傳感解決方案。

LibDriver SFA30是LibDriver推出的SFA30的全功能驅動，該驅動提供甲醛濃度讀取、溫度讀取、濕度讀取和SN讀取等功能並且它符合MISRA標準。

### 目錄

  - [說明](#說明)
  - [安裝](#安裝)
  - [使用](#使用)
    - [example basic](#example-basic)
  - [文檔](#文檔)
  - [貢獻](#貢獻)
  - [版權](#版權)
  - [聯繫我們](#聯繫我們)

### 說明

/src目錄包含了LibDriver SFA30的源文件。

/interface目錄包含了LibDriver SFA30與平台無關的IIC、UART總線模板。

/test目錄包含了LibDriver SFA30驅動測試程序，該程序可以簡單的測試芯片必要功能。

/example目錄包含了LibDriver SFA30編程範例。

/doc目錄包含了LibDriver SFA30離線文檔。

/datasheet目錄包含了SFA30數據手冊。

/project目錄包含了常用Linux與單片機開發板的工程樣例。所有工程均採用shell腳本作為調試方法，詳細內容可參考每個工程裡面的README.md。

/misra目錄包含了LibDriver MISRA程式碼掃描結果。

### 安裝

參考/interface目錄下與平台無關的IIC、UART總線模板，完成指定平台的IIC、UART總線驅動。

將/src目錄，您使用平臺的介面驅動和您開發的驅動加入工程，如果您想要使用默認的範例驅動，可以將/example目錄加入您的工程。

### 使用

您可以參考/example目錄下的程式設計範例完成適合您的驅動，如果您想要使用默認的程式設計範例，以下是它們的使用方法。

#### example basic

```C
#include "driver_sfa30_basic.h"

uint8_t res;
char sn[32];
uint32_t i;

/* init */
res = sfa30_basic_init(interface);
if (res != 0)
{
    return 1;
}

/* get the sn */
res = sfa30_basic_get_device_information((char *)sn);
if (res != 0)
{
    (void)sfa30_basic_deinit();

    return 1;
}

/* print */
sfa30_interface_debug_print("sfa30: sn is %s.\n", sn);

/* delay 2000 ms */
sfa30_interface_delay_ms(2000);

/* loop */
for (i = 0; i < times; i++)
{
    sfa30_data_t data;

    /* read data */
    res = sfa30_basic_read(&data);
    if (res != 0)
    {
        (void)sfa30_basic_deinit();

        return 1;
    }

    /* print */
    sfa30_interface_debug_print("sfa30: %d/%d.\n", i + 1, times);
    sfa30_interface_debug_print("sfa30: formaldehyde is %0.2fppb.\n", data.formaldehyde);
    sfa30_interface_debug_print("sfa30: humidity is %0.2f%%.\n", data.humidity);
    sfa30_interface_debug_print("sfa30: temperature is %0.2fC.\n", data.temperature);

    /* delay 2000 ms */
    sfa30_interface_delay_ms(2000);
}

/* deinit */
(void)sfa30_basic_deinit();

return 0;
```

### 文檔

在線文檔: [https://www.libdriver.com/docs/sfa30/index.html](https://www.libdriver.com/docs/sfa30/index.html)。

離線文檔: /doc/html/index.html。

### 貢獻

請參攷CONTRIBUTING.md。

### 版權

版權 (c) 2015 - 現在 LibDriver 版權所有

MIT 許可證（MIT）

特此免費授予任何獲得本軟件副本和相關文檔文件（下稱“軟件”）的人不受限制地處置該軟件的權利，包括不受限制地使用、複製、修改、合併、發布、分發、轉授許可和/或出售該軟件副本，以及再授權被配發了本軟件的人如上的權利，須在下列條件下：

上述版權聲明和本許可聲明應包含在該軟件的所有副本或實質成分中。

本軟件是“如此”提供的，沒有任何形式的明示或暗示的保證，包括但不限於對適銷性、特定用途的適用性和不侵權的保證。在任何情況下，作者或版權持有人都不對任何索賠、損害或其他責任負責，無論這些追責來自合同、侵權或其它行為中，還是產生於、源於或有關於本軟件以及本軟件的使用或其它處置。

### 聯繫我們

請聯繫lishifenging@outlook.com。