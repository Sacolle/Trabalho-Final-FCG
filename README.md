## Trabalho final de Fundamentos de Computação Gráfica
---
Este é o projeto final do grupo para cadeira de Fundamentos de Computação Gráfica da Universidade Federal do Rio Grande do Sul (UFRGS). O grupo é composto dos integrantes Pedro Henrique Boniatti Colle e Eduardo Dalmás Faé. 

O jogo desenvolvido será baseado no conceito de fuga de zumbis. Nele um jogador é colocado em uma vizinhança e deve se movimentar até o ponto de escape, que será um carro. A cada momento o número de zumbis perseguindo o jogador aumenta.

Busca-se, como extra no projeto, implementar formas de atrasar os zumbis e matá-los, assim como diferentes tipos de zumbis. Por fim, com tempo extra, a implementação da geração do mapa de forma automática, usando o algoritmo ‘wave function colapse’ seria de interesse da dupla.
Estilisticamente buca-se inspiração no jogo Minecraf, a short hike e Yiik a postodern rpg.

Roadmap de Implementação:
- [x] Realização do render na Tela.
- [x] Realização do render usando a perspectiva de uma câmera.
- [x] Implementação do Load de Meshes em .obj files.
	- [x] Vertices e índices em .obj com um único elemento
	- [ ] Carregamento dos demais atributos.
	- [ ] Carregamento de mais de um elemento por .obj file.
- [x] Implementação da estrutura de Entidades em Classes.
- [x] Implementação dos controles do player.
	- [x] Implemeção da movimentação.
	- [ ] Decouple do movimento do player (e Entidades) da Framerate.
	- [ ] Implemeção do pulo.
	- [ ] Implemeção de interações com o cenário e inimigos.
	- [ ] Implementação da interação Player-Camera.
- [x] Implementação da colisão de Entidades.
	- [x] Implementação das Bounding Boxes.
	- [x] Implementação dos Wire Frames para mostrar as Bounding Boxes.
	- [ ] Implementação das Funções de Colisão.
	- [ ] Implementação de um alritmo de Spacial Hash para gerir as colisões.
- [ ] Implementação do Game Loop.
	- [ ] Spawn de Inimigos.
	- [ ] Path finding dos zumbis usando o algoritmo A*.
	- [ ] Movimentação de alguma Entidade usando uma curva de Bezier.
- [ ] Implementação da geração de Terreno automático usando o algoritmo de *Wave Funcion Colapse*.
- [ ] Implementação do Menu e Tela de Pause
- [ ] Implementação do Som e Música.
- [ ] Implementação do Sistema de Iluminção.
	- [ ] Modelo difusa (Lamber).
	- [ ] Modelo Blinn-Phong.
	- [ ] Interpolação de Iluminação.
		- [ ] Modelo de Gouraud.
		- [ ] Modelo de Phong.
		
Algumas implementações são de requisito do projeto enquanto outras são de vontade do grupo. 