public class Rate {
    public static int rate(int legalN, int depth){
        int kingL=Chess.kingl(Chess.board);
        int rating=0,pieces=pieces();
        //add your rating
        rating+=pieces();
        rating+=location(pieces);
        rating+=moves(legalN,depth);
        Chess.doTheSwap();
        pieces=pieces();
        //substract the opponents rating to get my total rating in centipawns
        rating-=pieces();
        rating-=location(pieces);
        rating-=moves(legalN,depth);
        Chess.doTheSwap();
        return -(rating+depth*50);
    }
    public static int pieces(){
        //This method gives points for each piece based on chess convention
        int rating=0;
        for(int i=0;i<64;i++){
            switch(Chess.board[i/8][i%8]){
                case "K": rating+=20000;
                    break;
                case "Q": rating+=900;
                    break;
                case "R": rating+=500;
                    break;
                case "B": rating+=330;
                    break;
                case "N": rating+=320;
                    break;
                case "P": rating+=100;
                    break;
            }
        }
        return rating;
    }
    public static int location(int pieces){
        int rating=0;
        int pawnB[][]={//http://chessprogramming.wikispaces.com/Simplified+evaluation+function
        { 0,  0,  0,  0,  0,  0,  0,  0},
        {50, 50, 50, 50, 50, 50, 50, 50},
        {10, 10, 20, 30, 30, 20, 10, 10},
        { 5,  5, 10, 25, 25, 10,  5,  5},
        { 0,  0,  0, 20, 20,  0,  0,  0},
        { 5, -5,-10,  0,  0,-10, -5,  5},
        { 5, 10, 10,-20,-20, 10, 10,  5},
        { 0,  0,  0,  0,  0,  0,  0,  0}};
        int rookB[][]={
        { 0,  0,  0,  0,  0,  0,  0,  0},
        { 5, 10, 10, 10, 10, 10, 10,  5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        { 0,  0,  0,  5,  5,  0,  0,  0}};
        int nightB[][]={
        {-50,-40,-30,-30,-30,-30,-40,-50},
        {-40,-20,  0,  0,  0,  0,-20,-40},
        {-30,  0, 10, 15, 15, 10,  0,-30},
        {-30,  5, 15, 20, 20, 15,  5,-30},
        {-30,  0, 15, 20, 20, 15,  0,-30},
        {-30,  5, 10, 15, 15, 10,  5,-30},
        {-40,-20,  0,  5,  5,  0,-20,-40},
        {-50,-40,-30,-30,-30,-30,-40,-50}};
        int bishopB[][]={
        {-20,-10,-10,-10,-10,-10,-10,-20},
        {-10,  0,  0,  0,  0,  0,  0,-10},
        {-10,  0,  5, 10, 10,  5,  0,-10},
        {-10,  5,  5, 10, 10,  5,  5,-10},
        {-10,  0, 10, 10, 10, 10,  0,-10},
        {-10, 10, 10, 10, 10, 10, 10,-10},
        {-10,  5,  0,  0,  0,  0,  5,-10},
        {-20,-10,-10,-10,-10,-10,-10,-20}};
        int queenB[][]={
        {-20,-10,-10, -5, -5,-10,-10,-20},
        {-10,  0,  0,  0,  0,  0,  0,-10},
        {-10,  0,  5,  5,  5,  5,  0,-10},
        { -5,  0,  5,  5,  5,  5,  0, -5},
        {  0,  0,  5,  5,  5,  5,  0, -5},
        {-10,  5,  5,  5,  5,  5,  0,-10},
        {-10,  0,  5,  0,  0,  0,  0,-10},
        {-20,-10,-10, -5, -5,-10,-10,-20}};
        int kingMB[][]={
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-20,-30,-30,-40,-40,-30,-30,-20},
        {-10,-20,-20,-20,-20,-20,-20,-10},
        { 20, 20,  0,  0,  0,  0, 20, 20},
        { 20, 30, 10,  0,  0, 10, 30, 20}};
        int kingEB[][]={
        {-50,-40,-30,-20,-20,-30,-40,-50},
        {-30,-20,-10,  0,  0,-10,-20,-30},
        {-30,-10, 20, 30, 30, 20,-10,-30},
        {-30,-10, 30, 40, 40, 30,-10,-30},
        {-30,-10, 30, 40, 40, 30,-10,-30},
        {-30,-10, 20, 30, 30, 20,-10,-30},
        {-30,-30,  0,  0,  0,  0,-30,-30},
        {-50,-30,-30,-30,-30,-30,-30,-50}};
        for(int i=0;i<64;i++){
            switch(Chess.board[i/8][i%8]){
                case "P": rating+=pawnB[i/8][i%8];
                    break;
                case "N": rating+=nightB[i/8][i%8];
                    break;
                case "B": rating+=bishopB[i/8][i%8];
                    break;
                case "R": rating+=rookB[i/8][i%8];
                    break;
                case "Q": rating+=queenB[i/8][i%8];
                    break;
                case "K": if(pieces>=2000){rating+=kingMB[i/8][i%8];}else{rating+=kingEB[i/8][i%8];};
                    break;
            }
        }
        return rating;
    }
    public static int moves(int legalN,int d){
        int rating=0;
        rating+=legalN*50;
        if(legalN==0){
            if(!com.safeK()){
                rating+=-200000*d;
            }else{
                //rating+=-150000*d;
            }
        }
        return rating;
    }
}
