#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define MQ_PIN      0       // MQ-2 가스센서 (ADC0)
#define LED         PB0
#define FAN         PB2
#define BUZZER      PB3
#define ESP_SIGNAL  PB4     // ESP32-C3로 보내는 경보 신호
#define BUTTON      PD0     // 경보 해제 버튼 (INT0)

volatile uint8_t alarm_off = 0;   // 알람 해제 상태 플래그 (1이면 모든 출력 OFF)

void ADC_init(void)
{
    ADMUX  = (1 << REFS0);                      // 기준전압 AVCC(5V)
    ADCSRA = (1 << ADEN) |                      // ADC 활성화
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);   // 분주비 128
}

uint16_t ADC_read(uint8_t channel)
{
    ADMUX = (ADMUX & 0xF0) | channel;   // ADC 채널 선택
    ADCSRA |= (1 << ADSC);              // 변환 시작
    while (ADCSRA & (1 << ADSC));       // 변환 완료 대기
    return ADCW;                        // 10비트 결과 반환
}

ISR(INT0_vect)
{
    alarm_off = 1;   // 버튼 누르면 알람 해제
}

int main(void)
{
    uint16_t gas_value;
    uint16_t threshold = 350;   // MQ-2 경보 임계값

    // 출력 설정
    DDRB |= (1 << LED) | (1 << FAN) | (1 << BUZZER) | (1 << ESP_SIGNAL);

    // 버튼 입력, 내부 풀업 활성화
    DDRD &= ~(1 << BUTTON);
    PORTD |= (1 << BUTTON);

    // INT0 하강 에지 인터럽트 설정, 전역 인터럽트 허용
    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);
    sei();

    ADC_init();

    while (1)
    {
        gas_value = ADC_read(MQ_PIN);   // MQ-2 센서 값 읽기

        // 가스 농도가 기준치 이상이고 알람 해제 상태가 아닐 때
        if (gas_value > threshold && alarm_off == 0)
        {
            PORTB |= (1 << LED);
            PORTB |= (1 << FAN);
            PORTB |= (1 << BUZZER);
            PORTB |= (1 << ESP_SIGNAL);
        }
        else
        {
            PORTB &= ~(1 << LED);
            PORTB &= ~(1 << FAN);
            PORTB &= ~(1 << BUZZER);
            PORTB &= ~(1 << ESP_SIGNAL);
        }

        _delay_ms(30);   // 반응 속도를 위한 짧은 딜레이
    }
}
