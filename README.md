# 💧 Acompanhador de Consumo de Água com Raspberry Pi Pico W

Este projeto implementa um **webserver embarcado no Raspberry Pi Pico W** para incentivar hábitos de hidratação saudáveis. O sistema permite registrar quando o usuário bebeu água por meio de uma interface web, fornecendo feedback visual com LEDs, um display OLED, uma matriz de LEDs e feedback sonoro com buzzers passivos.

---

## 📸 Visão Geral

- Interface web simples com dois estados: **Feliz 😊** e **Triste 😞**
- Botão **"Bebi Água!"** que atualiza o estado para **Feliz**
- Se o botão não for pressionado por um período (configurável), o sistema muda automaticamente para o estado **Triste**
- Controle visual e sonoro via:
  - LEDs (verde/vermelho)
  - Display OLED SSD1306 (mensagens centralizadas)
  - Matriz de LEDs (atualmente apagada)
  - Buzzers passivos (beeps de alerta)

---

## 📦 Estrutura do Projeto

```
.
├── hydration_observer.c   # Código principal com lógica do servidor web, controle de estado e periféricos
└── lib/                   # Bibliotecas necessárias
    ├── ssd1306.h
    ├── ssd1306.c
    ├── font.h
    ├── pio_matrix.pio
    └── lwipopts.h
```

---

## 🧰 Requisitos

- Raspberry Pi Pico W  
- SDK do Raspberry Pi Pico (C/C++)  
- 2 LEDs comuns (verde e vermelho)  
- Display OLED SSD1306 (128x64, I2C, endereço 0x3C)  
- 2 buzzers passivos  
- Matriz de LEDs (25 LEDs)  
- Wi-Fi 2.4GHz  
- Um dispositivo com navegador web (smartphone ou computador)

---

## 🔧 Pinos Utilizados

| Periférico        | Pino Pico W |
|-------------------|-------------|
| LED Verde         | GP11        |
| LED Vermelho      | GP13        |
| I2C SDA (Display) | GP14        |
| I2C SCL (Display) | GP15        |
| Matriz de LEDs    | GP7         |
| Buzzer 1 (PWM)    | GP10        |
| Buzzer 2 (PWM)    | GP21        |

---

## ⚙️ Como Compilar e Executar

1. Clone o repositório:
   ```bash
   git clone https://github.com/Elmer-Carvalho/Hydration_Observer
   ```

2. Configure o SDK do Pico W no `CMakeLists.txt`

3. Compile com CMake:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. Grave o arquivo `.uf2` gerado no seu Raspberry Pi Pico W

---

## 🌐 Interface Web

A página web atualiza automaticamente a cada 10 segundos.

- **Estado Feliz 😄**  
  `😊 Muito bem!`

- **Estado Triste 😞**  
  `😞 Deixe de preguiça! Vá se hidratar!`

---

## 🔌 Funcionamento do Sistema

1. O usuário pressiona o botão **"Bebi Água!"** na interface web  
2. O servidor registra a ação e atualiza o estado para **Feliz**  
3. Se nenhum clique ocorrer em `TIME_LIMIT_MS` (padrão: 5 segundos para testes), o sistema muda para **Triste**

### Feedback visual e sonoro:

- **LEDs:**
  - GP11 (Verde) = Estado Feliz
  - GP13 (Vermelho) = Estado Triste

- **Display OLED:**
  - `"Hidratado!"` (Feliz)
  - `"Beba Água!"` (Triste)  
  Mensagens centralizadas na tela (128x64)

- **Buzzers:**
  - Quando o estado muda de Feliz → Triste
  - 2 beeps curtos em 440 Hz (200 ms de som, 100 ms de silêncio, 200 ms)

- **Matriz de LEDs:**
  - Atualmente apagada (GP7)

---

## 📡 Conectividade

O dispositivo conecta-se automaticamente à rede Wi-Fi definida por:

```c
#define WIFI_SSID "NOME DA SUA REDE"
#define WIFI_PASSWORD "SENHA DA SUA REDE"
```

Após a conexão, o servidor escuta na **porta 80**, e o IP é exibido no terminal via monitoramento serial.

---

## 🚀 Possíveis Expansões

- Armazenamento local do histórico de hidratação (ex.: usando Flash)
- Integração com sensores de fluxo ou botões físicos
- Controle por app mobile ou notificação via Telegram
- Estatísticas de consumo ao longo do tempo (usando gráficos)
- Uso da matriz de LEDs para indicar estados (ex.: acender em Feliz)
- Animações no display OLED (ex.: piscar mensagens)

---

## 👨‍💻 Autor

**Elmer Carvalho**  
Projeto embarcado para incentivo à hidratação com conectividade Wi-Fi, interface web, feedback visual e sonoro.

---

## 📜 Licença

Este projeto é de **uso educacional**.  
Modifique e compartilhe à vontade, com os devidos créditos.
