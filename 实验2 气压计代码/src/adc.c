#include "adc.h"
#include "adc12_a.h"
#include "timer.h"
#include "usart.h"
#include "gpio.h"

extern void ADC_Init(void)
{
    ADC12_A_configureMemoryParam ADC12_A12_configureMemoryParam;
    
    // Ch0~Ch7: P6.0~P6.7
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    
    // bool ADC12_A_init(uint16_t baseAddress, uint16_t sampleHoldSignalSourceSelect, uint8_t clockSourceSelect, uint16_t clockSourceDivider);
    // http://bbs.elecfans.com/jishu_547958_1_1.html 25.2.10这个章节中提到了一点，两个的采样周期必须大于30us。
    ADC12_A_init(ADC12_A_BASE, ADC12_A_SAMPLEHOLDSOURCE_SC, ADC12_A_CLOCKSOURCE_ACLK, ADC12_A_CLOCKDIVIDER_1);
    
    // #define ADC12_A_CYCLEHOLD_4_CYCLES                                (ADC12SHT0_0)
    // #define ADC12_A_CYCLEHOLD_8_CYCLES                                (ADC12SHT0_1)
    // #define ADC12_A_CYCLEHOLD_16_CYCLES                               (ADC12SHT0_2)
    // #define ADC12_A_CYCLEHOLD_32_CYCLES                               (ADC12SHT0_3)
    // #define ADC12_A_CYCLEHOLD_64_CYCLES                               (ADC12SHT0_4)
    // #define ADC12_A_CYCLEHOLD_96_CYCLES                               (ADC12SHT0_5)
    // #define ADC12_A_CYCLEHOLD_128_CYCLES                              (ADC12SHT0_6)
    // #define ADC12_A_CYCLEHOLD_192_CYCLES                              (ADC12SHT0_7)
    // #define ADC12_A_CYCLEHOLD_256_CYCLES                              (ADC12SHT0_8)
    // #define ADC12_A_CYCLEHOLD_384_CYCLES                              (ADC12SHT0_9)
    // #define ADC12_A_CYCLEHOLD_512_CYCLES                             (ADC12SHT0_10)
    // #define ADC12_A_CYCLEHOLD_768_CYCLES                             (ADC12SHT0_11)
    // #define ADC12_A_CYCLEHOLD_1024_CYCLES                            (ADC12SHT0_12)
    ADC12_A_setupSamplingTimer(ADC12_A_BASE, ADC12_A_CYCLEHOLD_1024_CYCLES, ADC12_A_CYCLEHOLD_1024_CYCLES, ADC12_A_MULTIPLESAMPLESENABLE);
    
    // Common parameter
    ADC12_A12_configureMemoryParam.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
    ADC12_A12_configureMemoryParam.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    ADC12_A12_configureMemoryParam.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
    
    // Sequence Ch0
    ADC12_A12_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_0;
    ADC12_A12_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A0;
    ADC12_A_configureMemory(ADC12_A_BASE, &ADC12_A12_configureMemoryParam);
    
    // Sequence Ch1
    ADC12_A12_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_1;
    ADC12_A12_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A1;
    ADC12_A_configureMemory(ADC12_A_BASE, &ADC12_A12_configureMemoryParam);
    
    // Sequence Ch2
    ADC12_A12_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_2;
    ADC12_A12_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A2;
    ADC12_A_configureMemory(ADC12_A_BASE, &ADC12_A12_configureMemoryParam);
    
    // Sequence Ch3
    ADC12_A12_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_3;
    ADC12_A12_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A3;
    ADC12_A_configureMemory(ADC12_A_BASE, &ADC12_A12_configureMemoryParam);
    
    // Sequence Ch4
    ADC12_A12_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_4;
    ADC12_A12_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A4;
    ADC12_A_configureMemory(ADC12_A_BASE, &ADC12_A12_configureMemoryParam);
    
    // Sequence Ch5
    ADC12_A12_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_5;
    ADC12_A12_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A5;
    ADC12_A_configureMemory(ADC12_A_BASE, &ADC12_A12_configureMemoryParam);
    
    // Sequence Ch6
    ADC12_A12_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_6;
    ADC12_A12_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A6;
    ADC12_A_configureMemory(ADC12_A_BASE, &ADC12_A12_configureMemoryParam);
    
    // Sequence Ch7
    ADC12_A12_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_7;
    ADC12_A12_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A7;
    ADC12_A12_configureMemoryParam.endOfSequence = ADC12_A_ENDOFSEQUENCE;
    ADC12_A_configureMemory(ADC12_A_BASE, &ADC12_A12_configureMemoryParam);
    
    ADC12_A_setResolution(ADC12_A_BASE, ADC12_A_RESOLUTION_12BIT);
    
    ADC12_A_setSampleHoldSignalInversion(ADC12_A_BASE, ADC12_A_NONINVERTEDSIGNAL);
    
    // ADC12_A_SIGNED_2SCOMPLEMENT or ADC12_A_UNSIGNED_BINARY
    ADC12_A_setDataReadBackFormat(ADC12_A_BASE, ADC12_A_UNSIGNED_BINARY);
    
    // 暂时不稀罕低功耗……
    ADC12_A_disableReferenceBurst(ADC12_A_BASE);
    
    // ADC12_A_MAXSAMPLINGRATE_200KSPS or ADC12_A_MAXSAMPLINGRATE_50KSPS
    ADC12_A_setReferenceBufferSamplingRate(ADC12_A_BASE, ADC12_A_MAXSAMPLINGRATE_200KSPS);
    
    // 开了中断就必须写中断处理函数 否则可能引起系统复位
    // Maybe ADC12_A_OVERFLOW_IE ...
    //ADC12_A_enableInterrupt(ADC12_A_BASE, ADC12_A_IE0|ADC12_A_IE1|ADC12_A_IE2|ADC12_A_IE3);
    ADC12_A_enable(ADC12_A_BASE);
    
    ADC12_A_startConversion(ADC12_A_BASE, ADC12_A_MEMORY_0, ADC12_A_REPEATED_SEQOFCHANNELS);
}

extern uint16 ADC_Ch(ADC_Ch_t ch)
{
    return ADC12_A_getResults(ADC12_A_BASE, ch);
}

// ADC
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector = ADC12_VECTOR
    __interrupt void ADC12ISR (void)
#elif defined(__GNUC__)
    void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
#else
    #error Compiler not supported!
#endif
{
    ;
}

