#include <ncursesw/curses.h>
#include <locale.h>
#include <iterator>
#include <string>
#include <algorithm>
#include <random>
#include <iostream>
#include <thread>
#include <chrono>
#include <cwchar>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdio>

#include "letters.h"


struct GameValues{

    bool GameOver = false;
    bool pause = false;

    int T[4] = {6,7,8,11};
    int O[4] = {6,7,10,11};
    int L[4] = {2,6,10,11};
    int J[4] = {3,7,11,10};
    int I[4] = {2,6,10,14};
    int Z[4] = {5,6,10,11};
    int S[4] = {7,8,10,11};


    int block_shape[4];
    int next_block_shape[4];

    const wchar_t* block_style = L"██";
    //const wchar_t* block_style = L"[]";

    const wchar_t* rect = L"██";


    int rotate;
    int next_rotate;

    int block_posX;
    int block_posY;
    int angles[4] = {0,90,180,270};

    //columns - 10  and rows -20

    static constexpr int columns = 14;
    static constexpr int rows = 24;

    int main_matrix[rows][columns] = {0};

    int field_posX;
    int field_posY;

    int addPointX = 5;
    int addPointY = 0;

    int test_value = 0;

    bool right_move = true;
    bool left_move = true;
    bool buttom_move = true;

    int color_num ;
    int block_num ;
    int next_color_num;
    int next_block_num;

    std::chrono::steady_clock::time_point lastFall =
        std::chrono::steady_clock::now();

    std::chrono::milliseconds fallDelay{400};

    int height;
    int width;
    int Lines = 0;
    int Lines2 = 0;
    int score = 0;

};


GameValues values;



void play_music()
{
    system("rm -f /tmp/tetris-mpv.sock");

    system(
        "mpv --no-video --loop=inf --really-quiet "
        "--input-ipc-server=/tmp/tetris-mpv.sock "
        "/home/sreenandh/LabRat/Rat8.0/audio/tetris.mp3 > /dev/null 2>&1 &"
    );
}

void stop_music()
{
    system("pkill -x mpv");
}


void pause_music()
{
    FILE* pipe = popen(
        "printf '{\"command\":[\"cycle\",\"pause\"]}\\n' | "
        "socat - UNIX-CONNECT:/tmp/tetris-mpv.sock > /dev/null 2>&1",
        "w"
    );

    if (pipe)
        pclose(pipe);
}
void rotation (int &index , int &rotate , int i , int j){
    if (rotate == 0){
        index = (i*4) + j + 1;
    }
    else if (rotate == 90) {
        index = 4*(3 - j) + 1 + i;
    }
    else if (rotate == 180) {
        index = 4*(3 - i) + 4 - j;
    }
    else if (rotate == 270 ) {
        index = 4*(1+j) - i ;
    }
}

void blocks_print(int (&shape)[4]){ //not needed

    int index;
    bool is_index_in = false;

    for (int i = 0 ; i < 4;i++){
        for (int j = 0 ; j < 4 ; j++){

            rotation(index , values.next_rotate , i ,j);

            for (int num : shape){
                if (index == num ) is_index_in = true;
            }

            if (is_index_in){
                //mvprintw(2 + i,2 + 3*j,std::to_string(index).c_str());
                attron(COLOR_PAIR(values.next_color_num));
                mvaddwstr(values.block_posY + i, values.block_posX + 2*j ,values.block_style );
                attroff(COLOR_PAIR(values.next_color_num));

                is_index_in = false;
            }
            else {
                mvprintw(values.block_posY + i,values.block_posX + 2*j, "  ");


            }


        }
    }

}







void add_blocks(int (&shape)[4]){  

    int index;
    

    for (int i = 0 ; i < 4;i++){
        for (int j = 0 ; j < 4 ; j++){

            bool is_index_in = false;

            rotation(index , values.rotate, i ,j);


            for (int num : shape){
                if (index == num ) is_index_in = true;
            }

            if (is_index_in){
                values.main_matrix[values.addPointY + i][values.addPointX + j] = values.color_num;
            }


        }
    }

}





void clean_while_input()
{
    int index;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {

            rotation(index , values.rotate ,i, j);

            for (int num : values.block_shape) {

                if (index == num) {

                    // Clear only  block
                    values.main_matrix
                        [values.addPointY + i]
                        [values.addPointX + j] = 0;

                    break;
                }
            }
        }
    }
}




bool can_move(int select)
{
    int dx = 0;
    int dy = 0;

    if (select == 0) {        // right
        dx = 1;
    }
    else if (select == 1) {   // left
        dx = -1;
    }
    else if (select == 2) {   // down
        dy = 1;
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int index;
            rotation(index ,values.rotate, i, j);

            bool occupied = false;

            for (int num : values.block_shape)
            {
                if (index == num)
                {
                    occupied = true;
                    break;
                }
            }

            if (!occupied)
                continue;

            int newY = values.addPointY + i + dy;
            int newX = values.addPointX + j + dx;

            // Boundary check
            if (newX < 0 || newX >= values.columns ||
                newY < 0 || newY >= values.rows)
            {
                return false;
            }

            if (values.main_matrix[newY][newX] != 0)
            {

                int otherIndex;
                rotation(otherIndex , values.rotate, i + dy, j + dx);

                bool sameBlock = false;

                for (int num : values.block_shape)
                {
                    if (otherIndex == num)
                    {
                        sameBlock = true;
                        break;
                    }
                }

                if (!sameBlock)
                    return false;
            }
        }
    }

    return true;
}




void Draw_tetris(){

    for (int i = 0; i < values.rows; i++ ){
        for (int j = 0 ; j < values.columns ; j++ ){

            if (j < 2 || j >= values.columns - 2 || i >= values.rows - 2 ){

                values.main_matrix[i][j] = 9;

                //mvprintw(values.field_posY + i, values.field_posX + 2*j ,"||" );

                if ( (j == 1 && i < values.rows -1  && i > 1)|| 
                        (j == values.columns - 2 && i < values.rows - 1 && i > 1)|| 
                        (i == values.rows - 2 && j > 1 && j < values.columns - 2)){
                    attron(COLOR_PAIR(7));
                    mvaddwstr(values.field_posY + i, values.field_posX + 2*j ,values.rect );
                    attroff(COLOR_PAIR(7));
                }

            }else if (values.main_matrix[i][j] != 0  && i >  1) {

                attron(COLOR_PAIR(values.main_matrix[i][j]));
                mvaddwstr(values.field_posY + i, values.field_posX + 2*j ,values.block_style );
                attroff(COLOR_PAIR(values.main_matrix[i][j]));
            }
            //else  mvprintw(values.field_posY + i, values.field_posX + 2*j ," ." );
            else if (values.main_matrix[i][j] == 0  && i > 1) {
                //mvprintw(values.field_posY + i, values.field_posX + 2*j ,".." );

                attron(COLOR_PAIR(2));
                mvaddwstr(values.field_posY + i, values.field_posX + 2*j ,L" . " );
                attroff(COLOR_PAIR(2));


            }
 

        }


    }


}

void draw_tetris_head(){

    int y =  (values.height - (values.rows )) /4;
    int x = (values.width - 38)/2;

    for (int i = 0 ; i < 3 ; i++){
        attron(COLOR_PAIR(1));
        mvaddwstr( y + i,x ,letter_T[i]);
        attroff(COLOR_PAIR(1));

    }

    for (int i = 0 ; i < 3 ; i++){
        attron(COLOR_PAIR(6));
        mvaddwstr( y + i ,x + 7 ,letter_E[i]);
        attroff(COLOR_PAIR(6));

    }

    for (int i = 0 ; i < 3 ; i++){
        attron(COLOR_PAIR(4));
        mvaddwstr( y + i ,x + 2*7 ,letter_T[i]);
        attroff(COLOR_PAIR(4));

    }
    for (int i = 0 ; i < 3 ; i++){
        attron(COLOR_PAIR(2));
        mvaddwstr( y + i ,x + 3*7 ,letter_R[i]);
        attroff(COLOR_PAIR(2));

    }
    for (int i = 0 ; i < 3 ; i++){
        attron(COLOR_PAIR(5));
        mvaddwstr( y + i ,x + 4*7 ,letter_I[i]);
        attroff(COLOR_PAIR(5));

    }

    for (int i = 0 ; i < 3 ; i++){
        attron(COLOR_PAIR(3));
        mvaddwstr( y + i ,x + 4.6*7 ,letter_S[i]);
        attroff(COLOR_PAIR(3));

    }

}


void draw_textbox(int width , int height, int color1 , int color2){

    int box_width = width*2;
    int color;
    //color1 = 8;
    //color2 = 9;
    int width1 = width * 0.8;
    int height1 = height *0.8;
    int box_width1 = width1*2;


    int posX1 = ((values.width - box_width)/2);
    int posY1 = (values.height - height)/2;

    int posX2 = (values.width - box_width1)/2;
    int posY2 = (values.height - height1)/2;

    
    for (int i = 0 ; i < height ; i++ ){
        color = 1;

        for (int j = 0 ; j < width ; j++){

            if ((j+i )%2 == 0 ) color = color1;
            else color = color2;
            
            attron( COLOR_PAIR(color)); 
            mvaddwstr(posY1+i ,posX1 + 2*j ,L"  ");
            attron( COLOR_PAIR(color));

            //color = color1;
        
                        

        }
    }

    for (int i = 0 ; i < (height1) ; i++ ){
        color = 1;

        for (int j = 0 ; j < (width1) ; j++){


            
            attron( COLOR_PAIR(color1)); 
            mvaddwstr(posY2+i ,posX2 + 2*j ,L"  ");
            attron( COLOR_PAIR(color1));

            //color = color1;
        
                        

        }
    }
   
}

void draw_line_point(){

    int y = values.field_posY;
    int x = values.width / 6;

    //attron(COLOR_PAIR(2));
    for (int i = 0 ; i < 3 ; i++){
        mvaddwstr( y + i,x ,letter_L[i]);

    }

    for (int i = 0 ; i < 3 ; i++){
        mvaddwstr( y + i ,x + 2 ,letter_i[i]);

    }

    for (int i = 0 ; i < 3 ; i++){
        mvaddwstr( y + i ,x + 3 ,letter_n[i]);

    }
    for (int i = 0 ; i < 3 ; i++){
        mvaddwstr( y + i ,x + 5 ,letter_e[i]);

    }
    for (int i = 0 ; i < 3 ; i++){
        mvaddwstr( y + i ,x + 7 ,letter_dot[i]);

    }

    for (int i = 0 ; i < 3 ; i++){
        mvaddwstr( y + i ,x + 8,number_0[i]);

    }
    //attroff(COLOR_PAIR(2));

}

void tecture(int color1 , int color2){
    int color;

    for (int i = 0 ; i < 2*values.width ; i++){
        for (int j = 0 ; j < 2*values.height ; j++ ){
            if ((i + j) % 2 == 0 ) color = color1;
            else color = color2;

            attron( COLOR_PAIR(color)); 
            mvaddwstr(i ,2*j ,L"  ");
            attron( COLOR_PAIR(color));


        }
    }
}





void Generate_block (int &color_num , int &rotate , int &block_num , int (&block_shape)[4]){
    std::random_device rd;
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<int> distrib1(1, 7);
    std::uniform_int_distribution<int> distrib2(0, 3);  //for angle 
    std::uniform_int_distribution<int> distrib3(1, 6); 
    int random_num1 = distrib1(gen);

    color_num = distrib3(gen);
    rotate = values.angles[distrib2(gen)];

    switch (random_num1){
        case 1:

            std::copy(std::begin(values.L), std::end(values.L), std::begin(block_shape)); 
            block_num = 1;
            break;

        case 2:

            std::copy(std::begin(values.J), std::end(values.J), std::begin(block_shape)); 
            block_num = 2;
            break;

        case 3:
            std::copy(std::begin(values.I), std::end(values.I), std::begin(block_shape)); 
            block_num = 3;
            break;

        case 4:
            std::copy(std::begin(values.O), std::end(values.O), std::begin(block_shape)); 
            block_num = 4;
            break;

       case 5:
            std::copy(std::begin(values.T), std::end(values.T), std::begin(block_shape)); 
            block_num = 5;
            break;

       case 6:
            std::copy(std::begin(values.Z), std::end(values.Z), std::begin(block_shape)); 
            block_num = 6;
            break;

       default:
            std::copy(std::begin(values.S), std::end(values.S), std::begin(block_shape)); 
            block_num = 7;
            break;



    }
    //values.rotate = 0;
    //std::copy(std::begin(values.S), std::end(values.S), std::begin(block_shape));
    //values.block_num = 7;

}




void print_matrix(){ //not needed


    for (int i = 0 ; i < values.rows ; i++){
        for (int j = 0 ; j < values.columns ; j++){

         mvprintw(2 + i,60 + 2 * j,std::to_string(values.main_matrix[i][j]).c_str());

        }
    }
}
 void rotate_adjust(){

    switch (values.block_num){
        case 1:
            if (values.rotate == 0 && !can_move(0)){
                values.addPointX -= 1;
            }
            if (values.rotate == 180 && !can_move(1)){
                values.addPointX += 1;
            }

            break;

        case 2:
            if (values.rotate == 0 && !can_move(0)){
                values.addPointX -= 1;
            }
            if (values.rotate == 180 && !can_move(1)){
                values.addPointX += 1;
            }


            break;

        case 3:
            if (values.rotate == 0 && !can_move(0)){
                values.addPointX -= 1;
            }
            else if (values.rotate == 180 && !can_move(0)){
                values.addPointX -= 1;

            }

            else if (values.rotate == 0 && !can_move(1)){
                values.addPointX += 1;
            }

            else if (values.rotate == 180 && !can_move(1)){
                values.addPointX += 2;
            }


            break;

        case 4: //nothing to do 
            break;

        case 5 :
            if (values.rotate == 270 && !can_move(0)){
                values.addPointX -= 1;
            }

            if (values.rotate == 90 && !can_move(1)){
                values.addPointX += 1;
            }


            break;
        case 6 :
            if (values.rotate == 90 && !can_move(0)){
                values.addPointX -= 1;
            }

            if (values.rotate == 270 && !can_move(1)){
                values.addPointX += 1;
            }


            break;

        default:
            if (values.rotate == 270 && !can_move(0)){
                values.addPointX -= 1;
            }

            if (values.rotate == 90 && !can_move(1)){
                values.addPointX += 1;
            }


            break;
    }
 }

void falling(){
    auto now = std::chrono::steady_clock::now();

    if(now - values.lastFall >= values.fallDelay && can_move(2)){
        clean_while_input();
        values.addPointY++;

        values.lastFall = now;
    }
    

}

void clearing_lines()
{
    for (int i = values.rows - 3; i >= 3; i--)
    {
        bool full = true;

        for (int j = 2; j < values.columns - 2; j++)
        {
            if (values.main_matrix[i][j] == 0)
            {
                full = false;
                break;
            }
        }

        if (full && ! can_move(2))
        {
            for (int k = i; k > 3; k--)
            {
                for (int j = 2; j < values.columns - 2; j++)
                {
                    values.main_matrix[k][j] =
                        values.main_matrix[k - 1][j];
                }
            }

            for (int j = 2; j < values.columns - 2; j++)
            {
                values.main_matrix[3][j] = 0;
            }
            i++;
        }
        if(full && !can_move(2)) values.Lines++ ;
    }
}








class GameEngine{
private:
    int  key = ERR;
    

public:
    GameEngine(){
        setlocale(LC_ALL, "");
        initscr();
        noecho();
        curs_set(0);
        nodelay(stdscr, TRUE);
        keypad(stdscr, TRUE);

        play_music();
        
        if(has_colors()){
            start_color();
            use_default_colors();


            init_pair(1,COLOR_RED,-1);
            init_pair(2,COLOR_GREEN,-1); 
            init_pair(3,COLOR_BLUE,-1); 
            init_pair(4,COLOR_YELLOW,-1); 
            init_pair(5,COLOR_CYAN,-1);
            init_pair(7, COLOR_MAGENTA, -1);
            init_pair(8, COLOR_GREEN, COLOR_RED);
            init_pair(9,COLOR_RED, COLOR_GREEN);
            init_pair(10, COLOR_WHITE, -1);
            init_pair(11,COLOR_CYAN,COLOR_BLUE);
            init_pair(12,COLOR_BLUE,COLOR_CYAN);

            if (can_change_color()) {
                init_color(202, 1000, 700, 0);   // orange 
                init_pair(6, 202, -1);


            }



        }


        Generate_block(values.color_num , values.rotate , values.block_num , values.block_shape); //current shape
        Generate_block(values.next_color_num , values.next_rotate , values.next_block_num , values.next_block_shape); //next shape
    }
    void Update(){
        //if (values.block_num == 3 && values.rotate == 0) values.rotate = 180; 


      
 

        if (key == 'w' || key == 'W' || key == KEY_UP ){ // for rotate
                                                         //
            if ((can_move(0) || can_move(1)) || values.block_num == 3){
                
                clean_while_input();
                rotate_adjust();
                int current_index  = values.rotate/90;
                int next_index = (current_index + 1) % 4;

                values.rotate = values.angles[next_index];
                if (values.block_num == 3 && values.rotate == 0) values.rotate = 180; 
            }

        }


        if ((key == 'd' || key == 'D' || key == KEY_RIGHT )&& can_move(0)){  //move right 
            
            clean_while_input();
            values.addPointX++;
        }

        if ((key == 'a'  || key == 'A' || key == KEY_LEFT) && can_move(1)){ //move left
            
            clean_while_input();
            values.addPointX--;
        }

        if ((key == 's'  || key == 'S' || key == KEY_DOWN) && can_move(2) ){   //move down

            
            clean_while_input();
            //values.addPointY++;
            values.fallDelay = std::chrono::milliseconds(30);
        }


        falling();
        add_blocks( values.block_shape); // adding the blocks to the main matrix
        clearing_lines();

        int diff = values.Lines - values.Lines2;

        if (diff != 0){
            if (diff != 4){
                values.score += diff*100;
            }
            else values.score += diff*200;
        }

        values.Lines2 = values.Lines;
        

        if(!can_move(2)){
            values.color_num = values.next_color_num;
            values.rotate = values.next_rotate;
            values.block_num = values.next_block_num;
            std::copy(std::begin(values.next_block_shape), std::end(values.next_block_shape), std::begin(values.block_shape)); 

            Generate_block(values.next_color_num , values.next_rotate , values.next_block_num , values.next_block_shape); //next shape
            values.addPointX = 5;
            values.addPointY = 0;
            if (!can_move(2)) values.GameOver = true;

            values.fallDelay = std::chrono::milliseconds(400);
        }

        
    }

    void Render(){
        erase();
        getmaxyx(stdscr, values.height, values.width);



        draw_tetris_head();
        draw_line_point();

        

        values.field_posX = (values.width - (values.columns*2 - 4))/2;
        values.field_posY = (values.height - (values.rows )) /2;

        Draw_tetris();

        values.block_posX = ((values.width - (values.columns*2 - 4))/2) - 10; 
        values.block_posY = ((values.height - (values.rows )) /2) +  6;
     
        blocks_print(values.next_block_shape);

        mvprintw(0, 0, "Terminal: %d x %d", values.width, values.height);
        mvprintw(2, 0, "Line : %d", values.Lines);
        mvprintw(3, 0, "Score : %d", values.score);


        if(values.pause) draw_textbox(30,20,11,12);
        




        refresh();
    }

    void Inputs(){
        key = getch();

        if (key == 'q' || key == 'Q') values.GameOver = true;   //key == 'q' or 'Q' for quit

        if (key == 'p' || key == 'P'){

            pause_music();
            values.pause = !values.pause;

        }

    }

    ~GameEngine(){ 
            
        stop_music();
        endwin();
    }

};


int main()
{




    GameEngine tetris;
    while (!values.GameOver){

        tetris.Inputs();
        if(!values.pause) tetris.Update();
        tetris.Render();

        napms(16); //60 fps
    }
    

    return 0;
}
