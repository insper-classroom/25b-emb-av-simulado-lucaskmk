# EXE3

![](imgs/exe3.gif)

Neste exercício vocês vão utilizar o RTOS para fazer dois LEDs piscarem. Porém o número de vezes que os LEDs irão piscar é definido por um dado que chega em uma fila. **O led amarelo só pode piscar depois do LED azul ter piscado**.

**Detalhes de funcionalidade:**

O código fornecido já possui uma fila `xQueueInput` que possui elementos de uma struct: 

```
 typedef struct input {
     int num_led1;
     int num_led2;
 } input_t;
```

- `num_led1`: Define quantas vezes o led azul deve piscar
- `num_led2`: Define quantas vezes o led amarelo deve piscar

O código também possui uma `input_task` que alimenta essa fila. Vocês não devem se preocupar em colocar dados da fila, apenas tirar!

Vocês devem criar três tasks de acordo com o diagrama a seguir:

![](imgs/diagrama2.png)

onde:

- `main_task`: Task que lê a fila `xQueueInput` e envia os dados para outras duas filas `xQueueLed1` e `xQueueLed2`.
- `led_1_task`: Task que faz o LED Azul piscar
- `led_2_task`: Task que faz o LED Amarelo piscar (**somente depois do LED azul parar**)
- `xQueueLed1`: Indica quantas vezes o LED azul deve piscar
- `xQueueLed2`: Indica quantas vezes o LED amarelo deve piscar
- `xSemaphoreLed2`: Semáforo que garante que o LED amarelo só vai piscar depois do LED azul.

**Detalhes do firmware:**

![](imgs/diagrama.png)

- Utulizar RTOS.
- Seguir estrutura proposta do firmware.
- Utilizar **período** de 500ms para piscar os LEDs.
- **printf** pode atrapalhar o tempo de simulação, comenta antes de testar.

## Testes

O código deve passar em todos os testes para ser aceito:

- `embedded_check`
- `firmware_check`
- `wokwi`

Caso acredite que o seu código está funcionando, porém os testes estão falhando, preencha o formulário:

[Google forms para revisão manual](https://docs.google.com/forms/d/e/1FAIpQLSdikhET4iqFwkOKmgD-G6Ri-2kCdhDLndlFWXdfdcuDfPnYHw/viewform?usp=dialog)
