
#ifndef _NRF24L01_H_
#define _NRF24L01_H_


/* Memory Map */
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD		0x1C
#define FEATURE		0x1D

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define CONT_WAVE   7
#define RF_DR_LOW   5
#define PLL_LOCK    4
#define RF_DR_HIGH  3
#define RF_PWR      1
#define LNA_HCURR   0
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
#define EN_DPL		2
#define EN_ACK_PAY	1
#define EN_DYN_ACK	0

/* Instruction Mnemonics */
/*
 #define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF
*/
/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define RF24_NOP      0xFF



// DYNDP
#define DPL_P5 5 // �������� ���� ������� ������������ ����� �� ������ 5
#define DPL_P4 4 // �������� ���� ������� ������������ ����� �� ������ 4
#define DPL_P3 3 // �������� ���� ������� ������������ ����� �� ������ 3
#define DPL_P2 2 // �������� ���� ������� ������������ ����� �� ������ 2
#define DPL_P1 1 // �������� ���� ������� ������������ ����� �� ������ 1
#define DPL_P0 0 // �������� ���� ������� ������������ ����� �� ������ 0



#define adr_3_bytes	0x01	//	����� ������
#define adr_4_bytes	0x02
#define adr_5_bytes	0x03































// SETUP_RETR
#define ARD 4 // 4 бита. Задаёт значение задержки перед повторной отправкой пакета: 250 x (n + 1) мкс
#define ARC 0 // 4 битай. Количество повторных попыток отправки, 0 - повторная отправка отключена.
#define SETUP_RETR_DELAY_250MKS (0 << ARD)
#define SETUP_RETR_DELAY_500MKS (1 << ARD)
#define SETUP_RETR_DELAY_750MKS (2 << ARD)
#define SETUP_RETR_DELAY_1000MKS (3 << ARD)
#define SETUP_RETR_DELAY_1250MKS (4 << ARD)
#define SETUP_RETR_DELAY_1500MKS (5 << ARD)
#define SETUP_RETR_DELAY_1750MKS (6 << ARD)
#define SETUP_RETR_DELAY_2000MKS (7 << ARD)
#define SETUP_RETR_DELAY_2250MKS (8 << ARD)
#define SETUP_RETR_DELAY_2500MKS (9 << ARD)
#define SETUP_RETR_DELAY_2750MKS (10 << ARD)
#define SETUP_RETR_DELAY_3000MKS (11 << ARD)
#define SETUP_RETR_DELAY_3250MKS (12 << ARD)
#define SETUP_RETR_DELAY_3500MKS (13 << ARD)
#define SETUP_RETR_DELAY_3750MKS (14 << ARD)
#define SETUP_RETR_DELAY_4000MKS (15 << ARD)
#define SETUP_RETR_NO_RETRANSMIT (0 << ARC)
#define SETUP_RETR_UP_TO_1_RETRANSMIT (1 << ARC)
#define SETUP_RETR_UP_TO_2_RETRANSMIT (2 << ARC)
#define SETUP_RETR_UP_TO_3_RETRANSMIT (3 << ARC)
#define SETUP_RETR_UP_TO_4_RETRANSMIT (4 << ARC)
#define SETUP_RETR_UP_TO_5_RETRANSMIT (5 << ARC)
#define SETUP_RETR_UP_TO_6_RETRANSMIT (6 << ARC)
#define SETUP_RETR_UP_TO_7_RETRANSMIT (7 << ARC)
#define SETUP_RETR_UP_TO_8_RETRANSMIT (8 << ARC)
#define SETUP_RETR_UP_TO_9_RETRANSMIT (9 << ARC)
#define SETUP_RETR_UP_TO_10_RETRANSMIT (10 << ARC)
#define SETUP_RETR_UP_TO_11_RETRANSMIT (11 << ARC)
#define SETUP_RETR_UP_TO_12_RETRANSMIT (12 << ARC)
#define SETUP_RETR_UP_TO_13_RETRANSMIT (13 << ARC)
#define SETUP_RETR_UP_TO_14_RETRANSMIT (14 << ARC)
#define SETUP_RETR_UP_TO_15_RETRANSMIT (15 << ARC)
// RF_SETUP
#define CONT_WAVE 7 // (Только для nRF24L01+) Непрерывная передача несущей (для тестов)
#define RF_DR_LOW 5 // (Только для nRF24L01+) Включает скорость 250кбит/с. RF_DR_HIGH должен быть 0
#define PLL_LOCK 4 // Для тестов
#define RF_DR_HIGH 3 // Выбор скорости обмена (при значении бита RF_DR_LOW = 0): 0 - 1Мбит/с; 1 - 2Мбит/с
#define RF_PWR 1 // 2бита. Выбирает мощность передатчика: 0 - -18dBm; 1 - -16dBm; 2 - -6dBm; 3 - 0dBm
#define RF_SETUP_MINUS18DBM (0 << RF_PWR)
#define RF_SETUP_MINUS12DBM (1 << RF_PWR)
#define RF_SETUP_MINUS6DBM (2 << RF_PWR)
#define RF_SETUP_0DBM (3 << RF_PWR)
#define RF_SETUP_1MBPS (0 << RF_DR_HIGH)
#define RF_SETUP_2MBPS (1 << RF_DR_HIGH)
#define RF_SETUP_250KBPS (1 << RF_DR_LOW) // этот режим не должен использоваться с контролем доставки
// STATUS
#define RX_DR 6 // Флаг получения новых данных в FIFO приёмника. Для сброса флага нужно записать 1
#define TX_DS 5 // Флаг завершения передачи. Для сброса флага нужно записать 1
#define MAX_RT 4 // Флаг превышения установленного числа повторов. Без сброса (записать 1) обмен невозможен
#define RX_P_NO 1 // 3 бита. Номер канала, данные для которого доступны в FIFO приёмника. 7 - FIFO пусто.
#define TX_FULL_STATUS 0 // Признак заполнения FIFO передатчика: 1 - заполнено; 0 - есть доступные слоты
// (переименовано из TX_FULL во избежание путаницы с одноимённым битом из регистра FIFO_STATUS)
// OBSERVE_TX
#define PLOS_CNT 4 // 4 бита. Общее количество пакетов без подтверждения. Сбрасывается записью RF_CH
#define ARC_CNT 0 // 4 бита. Количество предпринятых повторов при последней отправке.
// FIFO_STATUS
#define TX_REUSE 6 // Признак готовности последнего пакета для повторной отправки.
#define TX_FULL_FIFO 5 // Флаг переполнения FIFO очереди передатчика.
// (переименовано из TX_FULL во избежание путаницы с одноимённым битом из регистра STATUS)
#define TX_EMPTY 4 // Флаг освобождения FIFO очереди передатчика.
#define RX_FULL 1 // Флаг переполнения FIFO очереди приёмника.
#define RX_EMPTY 0 // Флаг освобождения FIFO очереди приёмника.
// DYNDP
#define DPL_P5 5 // Включает приём пакетов произвольной длины по каналу 5
#define DPL_P4 4 // Включает приём пакетов произвольной длины по каналу 4
#define DPL_P3 3 // Включает приём пакетов произвольной длины по каналу 3
#define DPL_P2 2 // Включает приём пакетов произвольной длины по каналу 2
#define DPL_P1 1 // Включает приём пакетов произвольной длины по каналу 1
#define DPL_P0 0 // Включает приём пакетов произвольной длины по каналу 0
// FEATURE
#define EN_DPL 2 // Включает поддержку приёма и передачи пакетов произвольной длины
#define EN_ACK_PAY 1 // Разрешает передачу данных с пакетами подтверждения приёма
#define EN_DYN_ACK 0 // Разрешает использование W_TX_PAYLOAD_NOACK



#define SETUP_AW_3BYTES_ADDRESS (1 << AW)
#define SETUP_AW_4BYTES_ADDRESS (2 << AW)
#define SETUP_AW_5BYTES_ADDRESS (3 << AW)





#endif // _NRF24L01_H_
