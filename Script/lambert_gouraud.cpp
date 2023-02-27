#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <iostream>

#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"
// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
glm::vec4 position_world = glm::vec4();
glm::vec4 normal;

// Matrizes computadas no código C++ e enviadas para a GPU
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
int object_id;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
glm::vec3 color;

int main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    glm::vec4 origin = glm::vec4(0.0, 0.0, 0.0, 1.0);
    glm::vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    
    /*
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
                                                                Mudar aqui se a questao for diferente
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
    */

    glm::vec4 x = glm::vec4(1, 0, 0, 0);
    glm::vec4 y = glm::vec4(0, 1, 0, 0);
    glm::vec4 z = glm::vec4(0, 0, 1, 0);

    glm::vec4 o = glm::vec4(0, 0, 0, 1);

    glm::vec4 a = o + 0.9f * x + 2.5f * y + 8.7f*z;
    glm::vec4 b = o + 7.6f * x + 2.3f * y + 7.5f*z;
    glm::vec4 c = o + 5.9f * x + 8.3f * y + 1.1f*z;

    glm::vec4 p = 0.2f*a + 0.2f*b + 0.6f*c;

    glm::vec4 na = normalize(-0.6f * y + -0.8f * z);
    glm::vec4 nb = normalize(-0.2f * x + -0.5f * y + -0.8f * z);
    glm::vec4 nc = normalize(-0.2f * x + -0.6f * y + -0.6f * z);

    glm::vec4 nt = glm::mix(na, nb, nc);

    glm::vec4 l1 = normalize(+x + y + z);
    glm::vec4 l2 = normalize(-x - y - z);
    glm::vec3 Il1 = glm::vec3(0.9f,0.9f,0.9f); // PREENCH AQUI o espectro da fonte de luz
    glm::vec3 Il2 = glm::vec3(0.9f,0.9f,0.9f);
    
    glm::vec3 Kd = glm::vec3(0.5f,0.2f,0.9f); // Refletância difusa

    glm::vec4 r1 = -l1 + 2.0f * nt *(dot(nt,l1)); // PREENCHA AQUI o vetor de reflexão especular ideal
    glm::vec4 r2 = -l2 + 2.0f * nt *(dot(nt,l2)); // PREENCHA AQUI o vetor de reflexão especular ideal
    
    /*
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
    */

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    glm::vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    glm::vec4 l = normalize(glm::vec4(1.0,1.0,0.5,0.0));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    glm::vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    glm::vec4 r = -l + 2.0f * n *(dot(n,l)); // PREENCHA AQUI o vetor de reflexão especular ideal

    // Parâmetros que definem as propriedades espectrais da superfície
     // Refletância difusa
    glm::vec3 Ks; // Refletância especular
    glm::vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong


    // Espectro da fonte de iluminação
    glm::vec3 I = glm::vec3(1.0,1.0,1.0); // PREENCH AQUI o espectro da fonte de luz

    // Espectro da luz ambiente
    glm::vec3 Ia = glm::vec3(0.2,0.2,0.2); // PREENCHA AQUI o espectro da luz ambiente

    // Termo difuso utilizando a lei dos cossenos de Lambert
    //glm::vec4 lp1 = 
    float lambert1 = std::max(0.0f, dot(nt,l1));
    glm::vec3 lambert_diffuse_term1 = Kd * Il1 * lambert1; // PREENCHA AQUI o termo difuso de Lambert
    float lambert2 = std::max(0.0f, dot(nt,l2));
    glm::vec3 lambert_diffuse_term2 = Kd * Il2 * lambert2; // PREENCHA AQUI o termo difuso de Lambert

    // Termo ambiente
    glm::vec3 ambient_term = glm::vec3(Ka[0]*Ia[0],Ka[1]*Ia[1],Ka[2]*Ia[2]); // PREENCHA AQUI o termo ambiente

    // Termo especular utilizando o modelo de iluminação de Phong
    float phong = pow(std::max(0.0f,dot(r1,v)),q);
    glm::vec3 phong_specular_term  = glm::vec3(Ks[0]*I[0]*phong,Ks[1]*I[1]*phong,Ks[2]*I[2]*phong); // PREENCH AQUI o termo especular de Phong

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    //color.a = 1;

    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.
    color  = lambert_diffuse_term1 + lambert_diffuse_term2; //+ ambient_term + phong_specular_term;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    //color = pow(color, glm::vec3(1.0,1.0,1.0) / 2.2f);

    printf("Gouraud Shading - RGB --> [%f, %f, %f]",color.x, color.y, color.z);
    return 0;
}

