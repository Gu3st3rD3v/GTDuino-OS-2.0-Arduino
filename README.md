# Qual o objetivo desse projeto?
O objetivo do meu projeto é ter um sistema operacional 100% funcional no Arduino, (observação: esse sistema operacional é tão leve que roda em qualquer dispositivo com 2kb de ram). Ele tem funções básicas, não é possível fazer ligações, mandar SMS e não é possível ouvir musicas de um cartão micro sd. Esse projeto não é um projeto finalizado, então poderá conter modificações futuras. O sistema é open-source, qualquer pessoa pode usar e modificar esse sistema e personalizar ele do jeito que ela quiser. Pretendo adicionar mais conteúdos nesse sistema operacional. 

# Inspirações:
Minha principal inspiração para criar esse projeto foi o Nokia 3310, esse é um dispositivo móvel lançado em nos anos 2000 fabricado pela Nokia, na época ele foi uma febre, (tanto que ele é um dos celulares mais conhecidos da Nokia). Para fazer esse o sistema operacional GTDuino OS eu me inspirei no sistema operacional Symbian OS, mas também me inspirei no Terry A. Davis o desenvolvedores do sistema operacional Temple OS, Davis começou a desenvolver o TempleOS em meados de 2003. Ele inicialmente o batizara "J Operating System", mais tarde mudando o nome para "LoseThos", numa referência a uma cena do filme de 1986 Platoon. Outro nome utilizado por ele foi "SparrowOS" antes de finalmente decidir-se por "TempleOS". 

# O que é o arduino?
O Arduino é uma placa de controle/microcomputador desenvolvido por Massimo Banzi, David Cuartielles, Tom Igoe, Gianluca Martino e David Mellis em 2005 na Italia, o Arduino é open-source ou seja, qualquer um pode criar e melhorar o Arduino. Ele é totalmente reprogramável (você consegue editar os códigos dessa placa), o Arduino IDE usa como linguagem principal C e C++. O nome Arduino vem de um bar na comuna italiana de Ivrea, onde alguns dos fundadores do projeto costumavam se reunir. O bar foi nomeado após Arduíno de Ivrea, que foi o marquês de Ivrea e rei da Itália de 1002 à 1014.

# Funções:
As funções desse sistema operacional são funções simples, como Bloco de notas, Configurações, Musicas (as musicas são definidas no sistema operacional, ela funciona a partir de bips (pretendo melhorar isso), Music Maker (você pode clicar nos botões de 1 a 6 para reproduzir um som, você só consegue reproduzir sons, mas não consegue os salva-los), Geometry Pocket (uma versão adaptada da minha inteligência artificial para o GTDuino OS), Pré-Escrita (um mini game que desafia o usuário a fazer desafios propostos na tela, como "clique no botão 5, 3 vezes"), Among Us (uma versão do jogo disponível na steam, android, ios e consoles, adaptado para o GTDuino, nesse jogo você precisa de descobrir quem é o impostor, na versão adaptada, você é o impostor e precisa de eliminar todos os tripulantes), Dino Game (uma versão adaptada do jogo dino game do google chrome), Snake (é o clássico "jogo da cobrinha" dos celulares da Nokia, adaptado para rodar no sistema operacional GTDuino) e para finalizar Tetris, (o tetris é um jogo onde você precisa de completar uma camada, a cada camada completa você ganha pontos).

# Recursos Necessários:
Para você fazer o sistema funcionar você precisa de usar as seguintes peças: Arduino UNO, Tela OLED 0.96 (optimizado apenas para essa tela), Teclado de membrana numérico matriz 4x4 16 teclas, Alto falante ou Buzzer de bip, Bateria 9V (opcional). E o Arduino IDE para enviar o código.

# Display OLED 
VCC: Pino 3.3v
GND: Pino GND
SDA: Pino analógico A4
SCL: Pino analógico A5

# Teclado Matricial 4x4
Linha 1 (R1): Pino digital 9
Linha 2 (R2): Pino digital 8
Linha 3 (R3): Pino digital 7
Linha 4 (R4): Pino digital 6
Coluna 1 (C1): Pino digital 5
Coluna 2 (C2): Pino digital 4
Coluna 3 (C3): Pino digital 3
Coluna 4 (C4): Pino digital 2

# Buzzer
Pino + (Sinal): Pino digital 10
Pino - (GND): Pino GND

<img width="369" height="477" alt="image" src="https://github.com/user-attachments/assets/f3285ca1-5d0b-439f-b020-95a0a56f2ffc" />


# Bibliotecas Necessárias:
Adafruit GFX Library,
Keypad,
Adafruit SSD1306
