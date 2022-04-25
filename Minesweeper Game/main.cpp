#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Header/Global.hpp"
#include <iostream>
using namespace std;

    // Don't read this!!
    // Please don't!!
    // Your'e just gonna waste your time!!
    // This is irrelevant to the project

    // Really??
    // OK since you insist, but don't blame me

    /*
    A fungus (plural: fungi or funguses) is any member of the group of eukaryotic organisms 
    that includes microorganisms such as yeasts and molds, as well as the more familiar mushrooms. 
    These organisms are classified as a kingdom,[4] separately from the other eukaryotic kingdoms,
    which by one traditional classification include Plantae, Animalia, Protozoa, and Chromista.
    A characteristic that places fungi in a different kingdom from plants, bacteria, and some protists 
    is chitin in their cell walls. Fungi, like animals, are heterotrophs; they acquire their food by 
    absorbing dissolved molecules, typically by secreting digestive enzymes into their environment.
    */

int fieldState[HEIGHT][WIDTH];  // If the cell is open or not
int fieldNum[HEIGHT][WIDTH];    // The total number of bombs in adjacent cell
int fieldBombs[HEIGHT][WIDTH];  // Store where the bombs are
int fieldFlag[HEIGHT][WIDTH];   // Store where the flags

int excluded_area[HEIGHT][WIDTH]; // To prevent commiting suicide we exclude the adjacent cell from bomb placement

int game_start = 0; // Start the game after the first open cell
int game_state = 1; // To tell if the game is active
int mouse_x, mouse_y;   // Mouse hover position



// Render the Numbers
void render_fieldNum(sf::RenderWindow& window, sf::Font font){
        for(int x = 0; x < WIDTH; x++){
            for(int y = 0; y < HEIGHT; y++){
                sf::RectangleShape rect;
                rect.setFillColor(sf::Color(15,10,6));
                rect.setOutlineColor(sf::Color(30,30,30));
                rect.setOutlineThickness(-1);
                rect.setPosition(sf::Vector2f(x*SCALE, y*SCALE));
                rect.setSize(sf::Vector2f(SCALE,SCALE));
                window.draw(rect);
            }
        }

        for(int x = 0; x < WIDTH; x++){
            for(int y = 0; y < HEIGHT; y++){
                if(fieldNum[y][x] != 0){

                    sf::Text num(to_string(fieldNum[y][x]), font);
                    num.setFillColor(sf::Color(255,255,255));
                    num.setCharacterSize(20);
                    num.setPosition((x*SCALE)+9,(y*SCALE)+4);
                    window.draw(num);
                }
            }
        }
}

// Render Flag
void render_fieldFlag(sf::RenderWindow& window){
    sf::Texture t_flag;
    t_flag.loadFromFile("image/flag.png");
    for(int x = 0; x < WIDTH; x++){
        for(int y = 0; y < HEIGHT; y++){
            if(fieldFlag[y][x] == 1){

                sf::Sprite s_flag;
                s_flag.setTexture(t_flag);
                s_flag.setPosition(x*SCALE, y*SCALE);
                window.draw(s_flag);
            }
        }
    }
}

// Render the Bombs
void render_fieldBombs(sf::RenderWindow& window){
    sf::Texture t_bomb;
    t_bomb.loadFromFile("image/mines.png");
    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){
            if(fieldFlag[y][x] == 0){
                if(fieldBombs[y][x] != 0){
                    
                    sf::Sprite s_bomb;
                    s_bomb.setTexture(t_bomb);
                    s_bomb.setPosition(x*SCALE, y*SCALE);
                    window.draw(s_bomb);
                }
            }
        }
    }
}

// Render Cell Cover
void render_CellCover(sf::RenderWindow& window){
    int x,y;
    for(int x = 0; x < WIDTH; x++){
        for(int y = 0; y < HEIGHT; y++){
            if(fieldState[y][x] == 0){
                sf::RectangleShape rect;
                rect.setFillColor(sf::Color(5,150,0));
                rect.setPosition(sf::Vector2f(x*SCALE, y*SCALE));
                rect.setSize(sf::Vector2f(SCALE,SCALE));
                window.draw(rect);
            }
            else if(fieldBombs[y][x] == 2){
                sf::RectangleShape rect;
                rect.setFillColor(sf::Color(255,0,0));
                rect.setSize(sf::Vector2f(SCALE,SCALE));
                rect.setPosition(sf::Vector2f((x*SCALE), (y*SCALE)));
                rect.setOutlineColor(sf::Color(15,10,6));
                rect.setOutlineThickness(-2);
                window.draw(rect);
            }
        }
    }

    if(fieldState[mouse_y/SCALE][mouse_x/SCALE] != 1){  // Hover over a cell
        sf::RectangleShape rect;
        rect.setFillColor(sf::Color(40,180,35));
        rect.setPosition(sf::Vector2f((mouse_x/SCALE)*SCALE, (mouse_y/SCALE)*SCALE));
        rect.setSize(sf::Vector2f(SCALE,SCALE));
        window.draw(rect);
    }
}

// Recursion Function to open adjacent Cells
// I'm so good at this!!
void open_cell(int x, int y){
    x = x/SCALE;
    y = y/SCALE;

    if(fieldState[y][x] == 0 && fieldNum[y][x] != 0){fieldState[y][x] = 1;}
    
    else if(fieldState[y][x] == 0){
        fieldState[y][x] = 1;

        if(x == 0 && y == 0){ //{0,0}
            open_cell((x+1)*SCALE   ,y*SCALE);
            open_cell((x+1)*SCALE   ,(y+1)*SCALE);
            open_cell((x)*SCALE     ,(y+1)*SCALE);
        }
        else if(x == WIDTH -1 && y == HEIGHT -1){ // {WIDTH, HEIGHT}
            open_cell((x-1)*SCALE   ,(y-1)*SCALE);
            open_cell((x)*SCALE     ,(y-1)*SCALE);
            open_cell((x-1)*SCALE    ,(y)*SCALE);
        }
        else if(x == 0 && y == HEIGHT -1){  // {0, HEIGHT}
            open_cell((x)*SCALE   ,(y-1)*SCALE);
            open_cell((x+1)*SCALE ,(y-1)*SCALE);
            open_cell((x+1)*SCALE ,(y)*SCALE);
        }
        else if(x == WIDTH -1 && y == 0){ // {WIDTH, 0}
            open_cell((x-1)*SCALE   ,(y)*SCALE);
            open_cell((x-1)*SCALE ,(y+1)*SCALE);
            open_cell((x)*SCALE ,(y+1)*SCALE);
        }

        else if(x == 0){ // {x == 0}
            open_cell(x*SCALE       ,(y-1)*SCALE);
            open_cell((x+1)*SCALE   ,(y-1)*SCALE);
            open_cell((x+1)*SCALE   ,y*SCALE);
            open_cell(x*SCALE       ,(y+1)*SCALE);
            open_cell((x+1)*SCALE   ,(y+1)*SCALE);
        }
        else if(y == 0){ // {y == 0}
            open_cell((x-1)*SCALE   ,y*SCALE);
            open_cell((x+1)*SCALE   ,y*SCALE);
            open_cell((x-1)*SCALE   ,(y+1)*SCALE);
            open_cell(x  *SCALE     ,(y+1)*SCALE);
            open_cell((x+1)*SCALE   ,(y+1)*SCALE);
        }
        else if(x == WIDTH-1){ // {x == WIDTH}
            open_cell((x-1)*SCALE   ,(y-1)*SCALE);
            open_cell((x)*SCALE     ,(y-1)*SCALE);
            open_cell((x-1)*SCALE   ,y*SCALE);
            open_cell((x-1)*SCALE   ,(y+1)*SCALE);
            open_cell(x*SCALE       ,(y+1)*SCALE);
        }
        else if(y == HEIGHT-1){ // {y == HEIGHT}
            open_cell((x-1)*SCALE   ,(y-1)*SCALE);
            open_cell((x)*SCALE     ,(y-1)*SCALE);
            open_cell((x+1)*SCALE   ,(y-1)*SCALE);
            open_cell((x-1)*SCALE   ,y*SCALE);
            open_cell((x+1)*SCALE   ,y*SCALE);
        }
        

        else{ // {Center}
            open_cell((x-1)*SCALE   ,(y-1)*SCALE);
            open_cell((x)*SCALE     ,(y-1)*SCALE);
            open_cell((x+1)*SCALE   ,(y-1)*SCALE);
            open_cell((x-1)*SCALE   ,y*SCALE);
            open_cell((x+1)*SCALE   ,y*SCALE);
            open_cell((x-1)*SCALE   ,(y+1)*SCALE);
            open_cell((x)*SCALE     ,(y+1)*SCALE);
            open_cell((x+1)*SCALE   ,(y+1)*SCALE);
        }
    }
}

// Open Cell when slapped
void mouseEvent_Checker(sf::RenderWindow& window, int X, int Y){

    

    if(fieldBombs[Y/SCALE][X/SCALE] == 1){ // Game OVER, You Loss, DEAD, WEAK, Idiot, Stupid
        fieldBombs[Y/SCALE][X/SCALE] = 2; // Make the cell different so I can paint it REDDDDDDD
        game_state = 0; 
    }
    open_cell(X,Y);
    
}

void check_Endgame(sf::Music& music){

    int flag_bomb;
    int cell_open;

    for(int x = 0; x < WIDTH; x++){
        for(int y = 0; y < HEIGHT; y++){
            if(fieldBombs[y][x] == 1 && fieldFlag[y][x] == 1){
                flag_bomb ++;
            }
            if(fieldState[y][x] == 1){
                cell_open ++;
            }
        }
    }
    if(flag_bomb == 40 && cell_open == (WIDTH*HEIGHT)-40){
        game_state = 2;
        if (music.getStatus() != sf::Music::Playing)
            music.play();
    }
}

// Show the game over screen
void gameOver_Screen(sf::RenderWindow& window){
    render_fieldBombs(window);
    render_fieldFlag(window);
}

// I hate this function and other similar function
// please tell me there is a much better ways to do this
void exclude_cell(int x, int y){
    x = x/SCALE;
    y = y/SCALE;
    if(y == 0 && x == 0){ // TOP LEFT
        excluded_area[y][x] = 1;
        excluded_area[y][x+1]    = 1; // right
        excluded_area[y+1][x]    = 1; // bot
        excluded_area[y+1][x+1]  = 1; // bot right
    }
    else if(y == HEIGHT-1 && x == WIDTH-1){ // BOT RIGHT
        excluded_area[y][x] = 1;
        excluded_area[y-1][x-1]  = 1; // top left
        excluded_area[y-1][x]    = 1; // top
        excluded_area[y][x-1]    = 1; // left
    }
    else if(y == HEIGHT-1 && x == 0){ // BOT LEFT
        excluded_area[y][x] = 1;
        excluded_area[y-1][x]    = 1; // top
        excluded_area[y-1][x+1]  = 1; // top right
        excluded_area[y][x+1]    = 1; // right
    }
    else if(y == 0 && x == WIDTH-1){ // TOP RIGHT
        excluded_area[y][x] = 1;
        excluded_area[y][x-1]    = 1; // left
        excluded_area[y+1][x-1]  = 1; // bot left
        excluded_area[y+1][x]    = 1; // bot
    }

    else if(y == 0){ // TOP TOP
        excluded_area[y][x] = 1;
        excluded_area[y][x-1]    = 1; // left
        excluded_area[y][x+1]    = 1; // right

        excluded_area[y+1][x-1]  = 1; // bot left
        excluded_area[y+1][x]    = 1; // bot
        excluded_area[y+1][x+1]  = 1; // bot right
    }
    else if(y == HEIGHT-1){ // BOT BOT
        excluded_area[y][x] = 1;
        excluded_area[y-1][x-1]  = 1; // top left
        excluded_area[y-1][x]    = 1; // top
        excluded_area[y-1][x+1]  = 1; // top right

        excluded_area[y][x-1]    = 1; // left
        excluded_area[y][x+1]    = 1; // right
    }
    else if(x == 0){ // RIGHT RIGHT
        excluded_area[y][x] = 1;
        excluded_area[y][x+1]    = 1; // right
        excluded_area[y-1][x]    = 1; // top
        excluded_area[y+1][x]    = 1; // bot
        excluded_area[y-1][x+1]  = 1; // top right
        excluded_area[y+1][x+1]  = 1; // bot right
    }
    else if(x == WIDTH-1){ // LEFT LEFT
        excluded_area[y][x] = 1;
        excluded_area[y][x-1]    = 1; // left
        excluded_area[y-1][x]    = 1; // top
        excluded_area[y+1][x]    = 1; // bot
        excluded_area[y-1][x-1]  = 1; // top left
        excluded_area[y+1][x-1]  = 1; // bot left
    }
    else{
    excluded_area[y-1][x-1] = 1;
    excluded_area[y-1][x] = 1;
    excluded_area[y-1][x+1] = 1;

    excluded_area[y][x-1] = 1;
    excluded_area[y][x] = 1;
    excluded_area[y][x+1] = 1;

    excluded_area[y+1][x-1] = 1;
    excluded_area[y+1][x] = 1;
    excluded_area[y+1][x+1] = 1;
    }
}

// Let loose the DRAGON
void bombsAway(){
    int bombs=40;
    while(bombs > 0){
        int x = rand()%WIDTH;
        int y = rand()%HEIGHT;
        if(fieldBombs[y][x] == 0 && excluded_area[y][x] == 0){
            fieldBombs[y][x] = 1;
            bombs--;
        }
    }
}

// Burn the surrounding cells
void fillNumber(){
    for(int y=0; y < HEIGHT; y++){ 
        for(int x=0; x < WIDTH; x++){ 
            fieldState[y][x] = 0;
            if(fieldBombs[y][x] != 0){

                if(y == 0 && x == 0){ // TOP LEFT
                    fieldNum[y][x+1]    += 1; // right
                    fieldNum[y+1][x]    += 1; // bot
                    fieldNum[y+1][x+1]  += 1; // bot right
                }
                else if(y == HEIGHT-1 && x == WIDTH-1){ // BOT RIGHT
                    fieldNum[y-1][x-1]  += 1; // top left
                    fieldNum[y-1][x]    += 1; // top
                    fieldNum[y][x-1]    += 1; // left
                }
                else if(y == HEIGHT-1 && x == 0){ // BOT LEFT
                    fieldNum[y-1][x]    += 1; // top
                    fieldNum[y-1][x+1]  += 1; // top right
                    fieldNum[y][x+1]    += 1; // right
                }
                else if(y == 0 && x == WIDTH-1){ // TOP RIGHT
                    fieldNum[y][x-1]    += 1; // left
                    fieldNum[y+1][x-1]  += 1; // bot left
                    fieldNum[y+1][x]    += 1; // bot
                }

                else if(y == 0){ // TOP TOP
                    fieldNum[y][x-1]    += 1; // left
                    fieldNum[y][x+1]    += 1; // right

                    fieldNum[y+1][x-1]  += 1; // bot left
                    fieldNum[y+1][x]    += 1; // bot
                    fieldNum[y+1][x+1]  += 1; // bot right
                }
                else if(y == HEIGHT-1){ // BOT BOT
                    fieldNum[y-1][x-1]  += 1; // top left
                    fieldNum[y-1][x]    += 1; // top
                    fieldNum[y-1][x+1]  += 1; // top right

                    fieldNum[y][x-1]    += 1; // left
                    fieldNum[y][x+1]    += 1; // right
                }
                else if(x == 0){ // RIGHT RIGHT
                    fieldNum[y][x+1]    += 1; // right
                    fieldNum[y-1][x]    += 1; // top
                    fieldNum[y+1][x]    += 1; // bot
                    fieldNum[y-1][x+1]  += 1; // top right
                    fieldNum[y+1][x+1]  += 1; // bot right
                }
                else if(x == WIDTH-1){ // LEFT LEFT
                    fieldNum[y][x-1]    += 1; // left
                    fieldNum[y-1][x]    += 1; // top
                    fieldNum[y+1][x]    += 1; // bot
                    fieldNum[y-1][x-1]  += 1; // top left
                    fieldNum[y+1][x-1]  += 1; // bot left
                }

                else{
                    fieldNum[y-1][x-1]  += 1; // top left
                    fieldNum[y-1][x]    += 1; // top
                    fieldNum[y-1][x+1]  += 1; // top right

                    fieldNum[y][x-1]    += 1; // left
                    fieldNum[y][x+1]    += 1; // right

                    fieldNum[y+1][x-1]  += 1; // bot left
                    fieldNum[y+1][x]    += 1; // bot
                    fieldNum[y+1][x+1]  += 1; // bot right
                }
            }
        }
    }

}

// Main Renderrer
void Render_Screen(sf::RenderWindow& window, int X, int Y, sf::Font font){
    if(game_state == 2){
        render_fieldNum(window, font);
        render_CellCover(window);
        gameOver_Screen(window);
    }
    else if(game_state == 1){
        render_fieldNum(window, font);
        render_CellCover(window);
        render_fieldFlag(window);
    }
    else{
        render_fieldNum(window, font);
        render_CellCover(window);
        gameOver_Screen(window);
    }
}

int main(){
    string title = "MineSweeper";
    int r = rand()%100;
    if(r == 0){title = "MinceSweeper";} // Theres chance the title will be mispelled

    sf::RenderWindow window(sf::VideoMode(WIDTH*SCALE, HEIGHT*SCALE), title);

    sf::Font font;
    font.loadFromFile("font/arial.ttf");

    sf::Music music;
    if (!music.openFromFile("audio/winning_sound.wav"))
        return -1; // error

    int X, Y; // mouse click position

    while (window.isOpen())
    {
        check_Endgame(music);
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch(event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                
                case sf::Event::MouseButtonReleased:
                    switch(event.mouseButton.button){

                        case sf::Mouse::Left:   // Mouse Left Click
                            if(game_start == 0){
                                X = sf::Mouse::getPosition(window).x;
                                Y = sf::Mouse::getPosition(window).y;
                                exclude_cell(X,Y);

                                bombsAway();    // Setup mines
                                fillNumber();   // Put number clue

                                game_start = 1;
                            }
                            if(game_state == 1 && fieldFlag[mouse_y/SCALE][mouse_x/SCALE] != 1){
                                X = sf::Mouse::getPosition(window).x; // store value cause i need it to pass to Render_Screen() function
                                Y = sf::Mouse::getPosition(window).y; 
                                mouseEvent_Checker(window, X, Y);
                            }
                            break;

                        case sf::Mouse::Right:  // Mouse Right Click
                            if(game_state == 1 && fieldState[mouse_y/SCALE][mouse_x/SCALE] == 0){
                                int x = sf::Mouse::getPosition(window).x;   
                                int y = sf::Mouse::getPosition(window).y;

                                if(fieldFlag[y/SCALE][x/SCALE] == 0){fieldFlag[y/SCALE][x/SCALE] = 1;} //Flag On
                                else{fieldFlag[y/SCALE][x/SCALE] = 0;}  // Flag Off
                            }
                            break;
                    }
                    break;
            }
        }

        mouse_x = sf::Mouse::getPosition(window).x;
        mouse_y = sf::Mouse::getPosition(window).y;

        window.clear();
        Render_Screen(window, X, Y, font);
        window.display();
    }

    return 0;
}