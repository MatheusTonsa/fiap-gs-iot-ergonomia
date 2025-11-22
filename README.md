# Solução de IoT para Monitoramento de Ergonomia e Presença

## Projeto para a Global Solution da FIAP

Este projeto consiste em uma solução de Internet das Coisas (IoT) desenvolvida para monitorar e promover um ambiente de trabalho mais saudável e produtivo. O sistema utiliza um ESP32 e múltiplos sensores para coletar dados sobre a ergonomia e a presença do usuário, enviando-os em tempo real para a plataforma FIWARE em nuvem.

---

### Cenário de Uso

O sistema foi projetado para resolver problemas comuns em ambientes de escritório, onde profissionais passam longas horas em frente ao computador. Os principais problemas abordados são:
- **Má postura:** Sentar-se muito perto ou muito longe da tela, causando dores e fadiga.
- **Falta de pausas:** Esquecer de fazer intervalos regulares, o que afeta a saúde ocular e a concentração.
- **Gestão de presença:** Garantir que o profissional esteja em seu posto de trabalho durante o horário produtivo.

---

### Arquitetura Proposta

A solução é composta pelos seguintes elementos:

1.  **Dispositivo de Monitoramento (Hardware):**
    - **Placa:** ESP32 DevKit v1.
    - **Sensores:**
        - **Sensor Ultrassônico (HC-SR04):** Mede a distância entre o usuário e a tela.
        - **Sensor de Presença (PIR HC-SR501):** Detecta a presença física do usuário, evitando que o sistema seja enganado por objetos inanimados.
    - **Atuadores:**
        - **LED Verde:** Indica status seguro (distância correta, em pausa).
        - **LED Vermelho:** Indica um alerta (distância incorreta, ausência, fim de pausa).
        - **Buzzer:** Emite alarmes sonoros para alertas críticos.
    - **Interação:**
        - **Botão de Pausa:** Permite ao usuário iniciar manualmente um intervalo.
        - **Botão de Reset:** Permite ao usuário confirmar o retorno da pausa e silenciar o alarme.

2.  **Ambiente de Simulação:**
    - **Wokwi:** A simulação completa do hardware e do código foi realizada na plataforma online Wokwi, permitindo testes e desenvolvimento sem a necessidade de componentes físicos.

3.  **Plataforma em Nuvem (Backend):**
    - **FIWARE:** O ecossistema FIWARE, rodando em uma máquina virtual na **Microsoft Azure**, é utilizado como backend.
    - **Orion Context Broker:** Recebe, armazena e gerencia os dados da entidade `PostoDeTrabalho:001` via requisições HTTP.

---

### Diferenciais da Solução

- **Inteligência Anti-Fraude:** O uso combinado do sensor de distância e do sensor de presença garante que o sistema só monitore a ergonomia quando uma pessoa real está presente.
- **Monitoramento Proativo:** O sistema não apenas alerta sobre má postura, mas também chama ativamente o usuário de volta ao seu posto caso ele se ausente por um tempo prolongado.
- **Gestão Completa do Bem-Estar:** A solução aborda três pilares: ergonomia (distância), gestão de pausas e controle de presença, oferecendo um cuidado 360° com o colaborador.
- **Arquitetura Escalável:** A utilização do FIWARE permite que a solução seja facilmente escalável para monitorar múltiplos postos de trabalho em uma empresa, centralizando os dados em um único local.

---

### Principais Dificuldades e Desafios

- **Integração de Ponta a Ponta:** O maior desafio foi estabelecer a comunicação entre o simulador Wokwi e a instância do FIWARE na Azure, o que envolveu a configuração de Docker, a criação de regras de firewall na nuvem e a depuração de requisições HTTP.
- **Lógica de Múltiplos Alarmes:** Desenvolver uma lógica de software robusta para gerenciar os diferentes tipos de alarme (distância, ausência, pausa) sem que um entrasse em conflito com o outro foi um desafio crucial para o bom funcionamento do sistema.

---

### Evidências da Integração com o FIWARE

A imagem abaixo mostra os dados da entidade `PostoDeTrabalho:001` sendo consultados diretamente do Orion Context Broker na Azure. Os valores de `distancia`, `presenca` e `status` são atualizados em tempo real pelo ESP32.

**(AQUI VOCÊ DEVE COLOCAR O PRINT DA TELA DO NAVEGADOR MOSTRANDO O JSON DO FIWARE)**
Para adicionar a imagem:
1. Tire o print.
2. No GitHub, vá em "Issues" -> "New Issue".
3. Arraste e solte a imagem na caixa de texto. O GitHub fará o upload e te dará um link.
4. Copie o link e cole aqui usando o formato: `![Texto descritivo](link_da_imagem)`

---

### Link da Simulação e Código-Fonte

- **Link para a Simulação no Wokwi:** [Insira aqui o link público do seu projeto Wokwi]
- **Código-Fonte:** O código completo do ESP32 está na pasta `projeto_wokwi/` deste repositório.
