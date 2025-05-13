# 💧 Acompanhador de Consumo de Água com Raspberry Pi Pico W

Este projeto implementa um **webserver embarcado** no **Raspberry Pi Pico W** para incentivar hábitos de hidratação saudáveis. O sistema permite registrar quando o usuário bebeu água por meio de uma interface web, e fornece feedback visual com LEDs, um display OLED e uma matriz de LEDs.

---

## 📸 Visão Geral

- Interface web simples com dois estados: **Feliz 😊** e **Triste 😞**.
- Botão "Bebi Água!" que atualiza o estado para **Feliz**.
- Se o botão não for pressionado por um período (configurável), o sistema muda automaticamente para o estado **Triste**.
- Controle visual via:
  - LEDs (verde/vermelho)

---

## 📦 Estrutura do Projeto

- `hydration_observer.c`: Código principal com lógica do servidor web, controle de estado e periféricos.
- `./lib`: Pasta contendo todos os includes necessários.

---

## 🧰 Requisitos

- Raspberry Pi Pico W
- SDK do Raspberry Pi Pico (C/C++)
- 2 LEDs comuns (verde e vermelho)
- Wi-Fi 2.4GHz
- Um dispositivo capaz de se conectar a rede local e acessar um navegador (Smartphone ou Computador).

---

## 🔧 Pinos Utilizados

| Periférico      | Pino Pico W     |
|-----------------|------------------|
| LED Verde       | GP11             |
| LED Vermelho    | GP13             |
| I2C SDA         | GP14             |
| I2C SCL         | GP15             |
| Matriz de LEDs  | GP7              |

---

## ⚙️ Como Compilar e Executar

1. Clone o repositório:
   git clone https://github.com/Elmer-Carvalho/Hydration_Observer

2. Configure o SDK do Pico W no `CMakeLists.txt`.

3. Compile com CMake:
   mkdir build
   cd build
   cmake ..
   make

4. Grave o `.uf2` no seu Raspberry Pi Pico W.

---

## 🌐 Interface Web

A página web atualiza automaticamente a cada 10 segundos.

### Estado Feliz 😄

😊 Muito bem!

### Estado Triste 😞

😞 Deixe de preguiça! Vá se hidratar!

---

## 🔌 Funcionamento do Sistema

- O usuário pressiona "Bebi Água!" na interface web.
- O servidor registra a ação e atualiza o estado para **Feliz**.
- Se nenhum clique ocorrer em `TIME_LIMIT_MS` (por padrão, 5 segundos para testes), o sistema muda para o estado **Triste**.
- Os LEDs e o display refletem o estado atual:
  - LED Verde = Feliz
  - LED Vermelho = Triste

---

## 📡 Conectividade

O dispositivo conecta-se automaticamente à rede Wi-Fi definida por:

#define WIFI_SSID "NOME DA SUA REDE"
#define WIFI_PASSWORD "SENHA DA SUA REDE"

Após a conexão, o servidor escuta na porta 80, e o IP é exibido no terminal e pode ser verificado por meio de monitoramento serial.

---

## 🚀 Possíveis Expansões

- Armazenamento local do histórico de hidratação (ex: usando Flash).
- Integração com sensores de fluxo ou botões físicos.
- Controle por app mobile ou notificação via Telegram.
- Estatísticas de consumo ao longo do tempo (usando gráficos).

---

## 👨‍💻 Autor

**Elmer Carvalho**  
Projeto embarcado para incentivo à hidratação com conectividade Wi-Fi e interface web.

---

## 📜 Licença

Este projeto é de uso educacional. Modifique e compartilhe à vontade com os devidos créditos.
