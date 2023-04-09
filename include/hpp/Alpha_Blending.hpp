#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <immintrin.h>
#include <string.h>

void Make_Blending_Not_Great_Just_No_Avx_Bebra(sf::Image* background, sf::Image* foreground, int offset_x, int offset_y);
void Make_Bledning_Great_Again_Avx_No_Bebra(sf::Image* background, sf::Image* foreground, int offset_x, int offset_y);

const char IMG_1_path[] = "include/images/source/image_1.bmp";
const char IMG_2_path[] = "include/images/source/image_2.bmp";
const char IMG_reult_path[] = "include/images/result/result.jpg";

struct ARGB
{
    sf::Uint8 alpha;
    sf::Uint8 red;
    sf::Uint8 green;
    sf::Uint8 blue;
};

