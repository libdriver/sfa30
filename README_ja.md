[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver SFA30

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/sfa30/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

SFA30 は、誤警報率を最小限に抑えるために最適化された電気化学センシング技術に基づくホルムアルデヒド検出モジュールです。 SFA30 HCHO センサーはエタノールに対する交差感度が低く、低い ppb (ppb) 範囲で動作するように最適化されており、屋内暴露の WHO 基準値付近の低 HCHO 濃度を正確に検出します。 統合された Sensirion RHT センサーと統合されたマイクロプロセッサにより、変化する屋内環境でも信頼性の高いパフォーマンスが保証されます。 このセンサーは、保管、取り扱い、輸送中の悪環境条件に耐えることができます。高度な乾燥防止技術により、堅牢なセンサーの寿命は 6 年です。 各 SFA30 は工場出荷時に校正されており、I2C または UART インターフェイスを介してデバイスに簡単に統合できます。 消費電力が低いため、バッテリ駆動が可能です。 これにより、SFA30 は信頼性が高く、使いやすく、高品質な HCHO&RHT コンボ センシング ソリューションになります。

LibDriver SFA30 は、LibDriver によって起動される SFA30 のフル機能ドライバーです。 ホルムアルデヒド濃度読み取り、温度読み取り、湿度読み取り、SN読み取りなどの機能を提供します。 LibDriver は MISRA に準拠しています。

### 目次

  - [説明](#説明)
  - [インストール](#インストール)
  - [使用](#使用)
    - [example basic](#example-basic)
  - [ドキュメント](#ドキュメント)
  - [貢献](#貢献)
  - [著作権](#著作権)
  - [連絡して](#連絡して)

### 説明

/ srcディレクトリには、LibDriver SFA30のソースファイルが含まれています。

/ interfaceディレクトリには、LibDriver SFA30用のプラットフォームに依存しないIIC, UARTバステンプレートが含まれています。

/ testディレクトリには、チップの必要な機能を簡単にテストできるLibDriver SFA30ドライバーテストプログラムが含まれています。

/ exampleディレクトリには、LibDriver SFA30プログラミング例が含まれています。

/ docディレクトリには、LibDriver SFA30オフラインドキュメントが含まれています。

/ datasheetディレクトリには、SFA30データシートが含まれています。

/ projectディレクトリには、一般的に使用されるLinuxおよびマイクロコントローラー開発ボードのプロジェクトサンプルが含まれています。 すべてのプロジェクトは、デバッグ方法としてシェルスクリプトを使用しています。詳細については、各プロジェクトのREADME.mdを参照してください。

/ misraはLibDriver misraコードスキャン結果を含む。

### インストール

/ interfaceディレクトリにあるプラットフォームに依存しないIIC, UARTバステンプレートを参照して、指定したプラットフォームのIIC, UARTバスドライバを完成させます。

/src ディレクトリ、プラットフォームのインターフェイス ドライバー、および独自のドライバーをプロジェクトに追加します。デフォルトのサンプル ドライバーを使用する場合は、/example ディレクトリをプロジェクトに追加します。

### 使用

/example ディレクトリ内のサンプルを参照して、独自のドライバーを完成させることができます。 デフォルトのプログラミング例を使用したい場合の使用方法は次のとおりです。

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

### ドキュメント

オンラインドキュメント: [https://www.libdriver.com/docs/sfa30/index.html](https://www.libdriver.com/docs/sfa30/index.html)。

オフラインドキュメント: /doc/html/index.html。

### 貢献

CONTRIBUTING.mdを参照してください。

### 著作権

著作権（c）2015-今 LibDriver 全著作権所有

MITライセンス（MIT）

このソフトウェアおよび関連するドキュメントファイル（「ソフトウェア」）のコピーを取得した人は、無制限の使用、複製、変更、組み込み、公開、配布、サブライセンスを含む、ソフトウェアを処分する権利を制限なく付与されます。ソフトウェアのライセンスおよび/またはコピーの販売、および上記のようにソフトウェアが配布された人の権利のサブライセンスは、次の条件に従うものとします。

上記の著作権表示およびこの許可通知は、このソフトウェアのすべてのコピーまたは実体に含まれるものとします。

このソフトウェアは「現状有姿」で提供され、商品性、特定目的への適合性、および非侵害の保証を含むがこれらに限定されない、明示または黙示を問わず、いかなる種類の保証もありません。 いかなる場合も、作者または著作権所有者は、契約、不法行為、またはその他の方法で、本ソフトウェアおよび本ソフトウェアの使用またはその他の廃棄に起因または関連して、請求、損害、またはその他の責任を負わないものとします。

### 連絡して

お問い合わせくださいlishifenging@outlook.com。