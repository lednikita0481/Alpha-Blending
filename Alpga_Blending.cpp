#include "include/hpp/Alpha_Blending.hpp"

int BGRD_WIDTH = 0;
int BGRD_HEIGHTH = 0;
int FGRD_WIDTH = 0;
int FGRD_HEIGHTH = 0;

int main()
{
    sf::Image background;
    background.loadFromFile(IMG_1_path);
    BGRD_WIDTH = background.getSize().x;
    BGRD_HEIGHTH = background.getSize().y;

    const sf::Uint8* pixels_to_save_background = background.getPixelsPtr();
    sf::Uint8* start_background_pixels = (sf::Uint8*) calloc(4*BGRD_WIDTH*BGRD_HEIGHTH, sizeof(sf::Uint8));
    memcpy(start_background_pixels, pixels_to_save_background, 4*BGRD_HEIGHTH*BGRD_WIDTH*sizeof(sf::Uint8));
    

    sf::Image foreground;
    foreground.loadFromFile(IMG_2_path);
    FGRD_WIDTH = foreground.getSize().x;
    FGRD_HEIGHTH  =foreground.getSize().y;

    sf::RenderWindow window(sf::VideoMode(BGRD_WIDTH, BGRD_HEIGHTH), "Mandelbrot!", sf::Style::Close);

    sf::Texture texture;
    texture.loadFromImage(background);
    sf::Sprite img_sprite;
    img_sprite.setTexture(texture);

    sf::Clock clock;

    //for (int i = 0; i < 10000; i++) 
        Make_Bledning_Great_Again_Avx_No_Bebra(&background, &foreground, 200, 200);

    background.saveToFile(IMG_reult_path);
    sf::Time elapsed_time = clock.getElapsedTime();

    printf("Time for blending is %f\n", (elapsed_time.asSeconds()*1000)/10000);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {        
                window.close();
            }
        }
        
        texture.update(background);
        window.clear();
        window.draw (img_sprite);
        window.display();
    }
}


void Make_Blending_Not_Great_Just_No_Avx_Bebra(sf::Image* background, sf::Image* foreground, int offset_x, int offset_y)
{
    sf::Uint8* bgrd_pixels = (sf::Uint8*) background->getPixelsPtr();
    sf::Uint8* fgrd_pixels = (sf::Uint8*) foreground->getPixelsPtr();

    for (int y_fgrd = 0; y_fgrd < FGRD_HEIGHTH; y_fgrd++)
    {
        for (int x_fgrd = 0; x_fgrd < FGRD_WIDTH; x_fgrd++)
        {
            sf::Uint8* bgrd_pixel =  (bgrd_pixels + 4*((y_fgrd + offset_y)*BGRD_WIDTH + x_fgrd + offset_x));
            sf::Uint8* fgrd_pixel =  (fgrd_pixels + 4*(y_fgrd*FGRD_WIDTH + x_fgrd));

            sf::Uint8 alpha = *(fgrd_pixel+3);

            //(sf::Uint8)0xFF &
            *(bgrd_pixel) = ( ((*(fgrd_pixel) * alpha) + (*(bgrd_pixel+0) * (255 - alpha)))) >> 8;
            *(bgrd_pixel+1) = ( ((*(fgrd_pixel+1) * alpha) + (*(bgrd_pixel+1) * (255 - alpha)))) >> 8;
            *(bgrd_pixel+2) = ( ((*(fgrd_pixel+2) * alpha) + (*(bgrd_pixel+2) * (255 - alpha)))) >> 8;
        }
    }
}

void Make_Bledning_Great_Again_Avx_No_Bebra(sf::Image* background, sf::Image* foreground, int offset_x, int offset_y)
{
    sf::Uint8* bgrd_pixels_array = (sf::Uint8*) background->getPixelsPtr();
    sf::Uint8* fgrd_pixels_array = (sf::Uint8*) foreground->getPixelsPtr();

    __m256i shit_wrom_where_sub_alpha = _mm256_set1_epi16 (255);

    for (int y_fgrd = 0; y_fgrd < FGRD_HEIGHTH; y_fgrd++)
    {
        for (int x_fgrd = 0; x_fgrd < FGRD_WIDTH; x_fgrd+=4)
        {
            __m128i bgrd_pixels_8 = _mm_loadu_si128 ((__m128i*) (bgrd_pixels_array + 4*((y_fgrd + offset_y)*BGRD_WIDTH + x_fgrd + offset_x)));
            __m128i fgrd_pixels_8 = _mm_loadu_si128 ((__m128i*) (fgrd_pixels_array + 4*(y_fgrd*FGRD_WIDTH + x_fgrd)));
            //load massive of 4 pixel with 4 params


            __m256i fgrd_pixels_16 = _mm256_cvtepi8_epi16 (fgrd_pixels_8);
            __m256i bgrd_pixels_16 = _mm256_cvtepi8_epi16 (bgrd_pixels_8);
            //create zeros before bytes to have place for mul

            __m256i alpha_mask = _mm256_set_epi8(0x80, 14, 0x80, 14, 0x80, 14, 0x80, 14,
                                                 0x80,  6, 0x80,  6, 0x80,  6, 0x80,  6,
                                                 0x80, 14, 0x80, 14, 0x80, 14, 0x80, 14,
                                                 0x80,  6, 0x80,  6, 0x80,  6, 0x80,  6);

            __m256i alpha = _mm256_shuffle_epi8(fgrd_pixels_16, alpha_mask);
            //alpha in the same places as everything else to mul

            __m256i foreground_alpha_mul = _mm256_mullo_epi16 (fgrd_pixels_16, alpha);
            __m256i background_alpha_mul = _mm256_mullo_epi16 (_mm256_sub_epi16(shit_wrom_where_sub_alpha, alpha), bgrd_pixels_16);
                                                //muldilo

            __m256i summ_all = _mm256_add_epi16 (foreground_alpha_mul, background_alpha_mul);

            __m256i summ_mask = _mm256_set_epi8(15, 13, 11, 9, 7, 5, 3, 1,
                                                0x80,  0x80, 0x80,  0x80, 0x80,  0x80, 0x80,  0x80,
                                                0x80,  0x80, 0x80,  0x80, 0x80,  0x80, 0x80,  0x80,
                                                15, 13, 11, 9, 7, 5, 3, 1);

            __m256i summ = _mm256_shuffle_epi8 (summ_all, summ_mask);

            __m128i color = _mm_add_epi64(_mm256_extracti128_si256(summ, 1), _mm256_castsi256_si128(summ));
            _mm_store_si128 ((__m128i *) (bgrd_pixels_array + 4*((y_fgrd + offset_y)*BGRD_WIDTH + x_fgrd + offset_x)), color);

        }
    }
}