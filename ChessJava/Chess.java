//Adam Brown's personal project chess

import java.io.*;
import java.sql.Timestamp;
import java.util.Arrays;
import javax.swing.*;
//import java.util.Arrays;

public class Chess {
    
    //Array board representation
    //
    //Upper case = white
    //Lower case = black
    //
    //King represented by: K/k
    //Queen represented by: Q/q
    //Rook represented by: R/r
    //Bishop represented by: B/b
    //Knight represented by: N/n (king is already k so n will do)
    //Pawn represented by: P/p
    
    static String board[][]={
        {"r","n","b","q","k","b","n","r"}, //0-7
        {"p","p","p","p","p","p","p","p"}, //8-15
        {" "," "," "," "," "," "," "," "}, //16-23
        {" "," "," "," "," "," "," "," "}, //24-31
        {" "," "," "," "," "," "," "," "}, //32-39
        {" "," "," "," "," "," "," "," "}, //40-47
        {"P","P","P","P","P","P","P","P"}, //48-55
        {"R","N","B","Q","K","B","N","R"}};//56-63
//    static String board[][]={
//        {" "," "," "," ","k"," "," "," "},
//        {" "," "," ","P"," "," "," ","R"},
//        {" "," ","N"," "," "," "," ","Q"},
//        {" "," "," "," "," "," "," "," "},
//        {" "," "," "," "," "," "," "," "},
//        {" "," "," "," "," "," "," "," "},
//        {" "," "," "," "," "," "," "," "},
//        {" "," "," "," ","K"," "," "," "}};

    
    //The depth of the engine
    static int publicDepth=4,aBCounter=0,aBTime,turn;
    static String startTime;
    static boolean MOVE = true, qCastle = true, kCastle = true, QCastle = true, KCastle = true, canEP = false;
    public static void main(String[] args) throws IOException {
        //Get settings
        String[] settings = InOut.settings();
        System.out.println(Arrays.toString(settings));
        
        //Create a file to log too with a timestamp
        java.util.Date date= new java.util.Date();
	    startTime="Logs/"+new Timestamp(date.getTime()).toString()+".txt";
        
        //File names cannot include ':' to use ';' instead
        for(int i=0;i<startTime.length();i++){
            try {
                if (":".equals(startTime.substring(i, i + 1))) {
                    startTime = startTime.substring(0, i) + ";" + startTime.substring(i + 1);
                }
            } catch (Exception e) {}
        }
        try(PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(startTime, true)))){}catch (IOException e) {}
        
        //Difficuly setting
        if("Normal".equals(settings[1])){
            
        }else if("Easy".equals(settings[1])){
            publicDepth=1;
        }else{
            InOut.write("Incorrect Settings! Review the README.");
            throw new UnsupportedOperationException("Incorrect Settings!");
        }
        
        //Opening the window
        JFrame f=new JFrame(settings[0]);
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        ChessInterface ui = new ChessInterface();
        f.add(ui);
        f.setSize(696, 719);
        f.setVisible(true);
    }
    public static String[][] cloneArray(String[][] src) {
        int length = src.length;
        String[][] target = new String[length][src[0].length];
        for (int i = 0; i < length; i++) {
            System.arraycopy(src[i], 0, target[i], 0, src[i].length);
        }
        return target;
    }
    public static String numToString(int n){
        if(1==(int) Math.log10(n) + 1){
            return "0"+n;
        }else if(n==0){
            return "00";
        }else{
            return ""+n;
        }
    }
    public static String myTurn () throws IOException{
        System.out.println("It is my turn!");
        stalemate();
        doTheSwap();
        int kl=KINGL(board);
        //Get list of legal moves
        String moveList = listMoves();
        //If there are no legal moves
        if(moveList.equals("")){
            System.out.println("Checking end");
            checkEnd("Checkmate - You Won!");
            return"";
        }
        //Magic starts here
        long sTime = System.currentTimeMillis();
        String move;
        System.out.println("Starting aB");
        move = alphaBeta(publicDepth,1000000,-1000000,"",0);
        long endTime = System.currentTimeMillis();
        System.out.println("alphaBeta took " + (endTime - sTime) + " milliseconds");
        System.out.println(move);
        InOut.log(move);
        System.out.println("Checked this many "+aBCounter);
        aBCounter=0;
        move(move);
        //Ends here
        
        doTheSwap();
        //Check to see if the computer has won
        if(listMoves().equals("")){
            checkEnd("Checkmate - You Lost!");
            doTheSwap();
        }
        System.out.println("Your score is "+-Rate.rate(listMoves().length()/8,0));
        stalemate();
        MOVE = true;
        return move;
    }
    public static boolean LEGAL (String op, int o, int n){
        //Checks what piece is being moved then if that move for that piece is legal
        switch (op) {
            case "K":
                if(lK(o, n)){
                    
                    //Preventing castling if you've moved the piece already.
                    if(MOVE){
                        QCastle = false;
                        KCastle = false;
                    }else{
                        qCastle = false;
                        kCastle = false;
                    }
                    return true;
                }else {
                    return false;
                }
            case "Q":
                if (lQ(o, n)){
                    return true;
                }else {
                    return false;
            }
            case "R":
                if (lR(o, n)){
                    
                    //Preventing castling if you've moved the piece already.
                    if(o==56){
                        if(MOVE){
                            QCastle = false;
                        }else{
                            qCastle = false;
                        }
                    }else if(o==63){
                        if(MOVE){
                            KCastle = false;
                        }else{
                            kCastle = false;
                        }
                    }
                    
                    
                    return true;
                }else {
                    return false;
                }
            case "B":
                if (lB(o, n)){
                    return true;
                }else {
                    return false;
                }
            case "N":
                if (lN(o, n)){
                    return true;
                }else {
                    return false;
                }
            case "P":
                if (lP(o, n)){
                    return true;
                }else {
                    return false;
                }
            case "^":
                if (lP(o, n)){
                    return true;
                }else {
                    return false;
                }
            default:
                return false;
        }
    }
    public static boolean lK (int o, int n){
        int y = o/8, x = o%8, m;
        for (int i = 0; i < 9; i++){
            try{
                if (i != 4/*If the KING is not moving onto itself*/ ){
                    if (/*If the locations one space away from the KING are an enemy or a space*/Character.isLowerCase(board[y-1 + i/3][x-1 + i%3].charAt(0)) || board[y-1 + i/3][x-1 + i%3].equals(" ")){
                        //return the location of the new space
                            m = (y-1 + i/3)*8 + (x-1 + i%3);
                        }else {
                            m = -1;
                    }
                }else{
                       m = -1;
                }

                //If the desired location of the KING is ever the same as a legal location
                if (m == n){
                    if(safeK(x,y,n%8,n/8)){
                        return true; 
                    }
                }
                
            }catch(Exception exception){//If there is an exception then skip it
                double easterEgg = Math.PI;
            }
            
            //Castling
            if(o==60){
                if(n==62){
                    if(" ".equals(board[7][5])){
                        if(" ".equals(board[7][6])){
                            if("R".equals(board[7][7])){
                                if(safeK(x,y,n%8,n/8)){
                                    return true; 
                                }
                            }
                        }
                    }
                }else if(n==58){
                    if(" ".equals(board[7][3])){
                        if(" ".equals(board[7][2])){
                            if(" ".equals(board[7][1])){
                                if("R".equals(board[7][0])){
                                    if (safeK(x, y, n % 8, n / 8)) {
                                        return true;                                        
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }
    public static boolean lQ (int o, int n){
        int y = o/8, x = o%8, t = 1, ny, nx;
        boolean k = true;
        
        //Check for each direction of movement
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                while(k){
                    ny = y+i*t;
                    nx = x+j*t;
                    t++;
                    try{
                        if(board[ny][nx].equals(" ")){
                            if(n == ny*8 + nx){
                                if(safeK(x,y,nx,ny)){
                                    return true;
                                }
                            }
                        }else if(Character.isLowerCase(board[ny][nx].charAt(0))){
                            k = false;
                            if(n == ny*8 + nx){
                                if(safeK(x,y,nx,ny)){
                                    return true;
                                }
                            }
                        }else{
                            k = false;
                        }
                    }catch(Exception exception){
                        k = false;
                    }
                }
                k = true;
                t = 1;
            }
        }
        return false;
    }
    public static boolean lR (int o, int n){
        int y = o/8, x = o%8, t = 1, ny, nx;
        boolean k = true;
        
        //Check for each direction of movement
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                
                //Only continue if moving in a straight line
                if(i == 0 || j == 0){
                    while(k){
                        ny = y+i*t;
                        nx = x+j*t;
                        t++;
                        try{
                            if(board[ny][nx].equals(" ")){
                                if(n == ny*8 + nx){
                                    if(safeK(x,y,nx,ny)){
                                        return true;
                                    }
                                }
                            }else if(Character.isLowerCase(board[ny][nx].charAt(0))){
                                k = false;
                                if(n == ny*8 + nx){
                                    if(safeK(x,y,nx,ny)){
                                        return true;
                                    }
                                }
                            }else{
                                k = false;
                            }
                        }catch(Exception exception){
                            k = false;
                        }
                    }
                }
                k = true;
                t = 1;
            }
        }
        return false;
    }
    public static boolean lB (int o, int n){
        int y = o/8, x = o%8, t = 1, ny, nx;
        boolean k = true;
        
        //Check for each move direction
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                
                //Continue only if diagonal
                if(i != 0 && j != 0){

                    while(k){
                        ny = y+i*t;
                        nx = x+j*t;
                        t++;
                        try{
                            if(board[ny][nx].equals(" ")){
                                if(n == ny*8 + nx){
                                    if(safeK(x,y,nx,ny)){
                                        return true;
                                    }
                                }
                            }else if(Character.isLowerCase(board[ny][nx].charAt(0))){
                                k = false;
                                if(n == ny*8 + nx){
                                    if(safeK(x,y,nx,ny)){
                                        return true;
                                    }
                                }
                            }else{
                                k = false;
                            }
                        }catch(Exception exception){
                            k = false;
                        }
                    }
                }
                k = true;
                t = 1;
            }
        }
        return false;
    }
    public static boolean lN (int o, int n){
        int y = o/8, x = o%8, a=n/8,b=n%8;
        //Bit easier when you don't have check for piece collision.
        if(!Character.isUpperCase(board[n/8][n%8].charAt(0))){
            if(a==y-1&&b==x-2){
                if(safeK(x,y,n%8,n/8)){
                    return true;
                }
            }
            if(a==y-1&&b==x+2){
                if(safeK(x,y,n%8,n/8)){
                    return true;
                }
            }
            if(a==y+1&&b==x-2){
                if(safeK(x,y,n%8,n/8)){
                    return true;
                }
            }
            if(a==y+1&&b==x+2){
                if(safeK(x,y,n%8,n/8)){
                    return true;
                }
            }
            if(a==y-2&&b==x-1){
                if(safeK(x,y,n%8,n/8)){
                    return true;
                }
            }
            if(a==y-2&&b==x+1){
                if(safeK(x,y,n%8,n/8)){
                    return true;
                }
            }
            if(a==y+2&&b==x-1){
                if(safeK(x,y,n%8,n/8)){
                    return true;
                }
            }
            if(a==y+2&&b==x+1){
                if(safeK(x,y,n%8,n/8)){
                    return true;
                }
            }
        }
        return false;
    }
    public static boolean lP (int o, int n){
        int y = o/8, x=o%8;
            if((board[n/8][n%8].equals(" ") && (n == o-8 || n == o-16 && y == 6)) || Character.isLowerCase(board[n/8][n%8].charAt(0)) && (n == o-7 || n == o-9)){
                if(safeK(x,y,n%8,n/8)){
                    return true;
                }
            }
        return false;
    }
    public static void PROMOTE(){
        for(int i = 0; i < 8; i++){
            if(board[0][i].equals("P")){
                board[0][i] = "Q";
            }
        }
    }
    public static boolean safeK(int ox, int oy, int nx, int ny){
        String[][] boardTemp=cloneArray(board);
        board[ny][nx]=board[oy][ox];
        if(ox==4&&oy==7){
            if(nx==6&&ny==7){
                board[7][5]="R";
            }else if(nx==2&&ny==7){
                board[7][3]="R";
            }
        }
        board[oy][ox]=" ";
        int Kl = KINGL(board);
        int kx=Kl%8;
        int ky=Kl/8;
        boolean k = true;
        int t;
        for(int i=-1;i <= 1; i++){
            for(int j=-1; j<=1; j++){
                t=1;
                //Checking for queen/rook/bishop
                while(k){
                    try{
                        int cx = kx+j*t, cy = ky+i*t;
                        String b = board[cy][cx];
                        if(Character.isUpperCase(b.charAt(0))){
                            k=false;
                        }else if(Character.isLowerCase(b.charAt(0))){
                            k=false;
                            if(b.equals("q")){
                                board=boardTemp;
                                return false;
                            }else if (b.equals("r")&&(i==0||j==0)){
                                board=boardTemp;
                                return false;
                            }else if (b.equals("b")&&i!=0&&j!=0){
                                board=boardTemp;
                                return false;
                            }else{
                                
                            }
                        }else{
                            t++;
                        }
                    }catch(Exception exception){
                        k=false;
                    }
                    
                }
                k=true;
            }
        }
        
        //Checking for king
        for(int i=0;i<9;i++){
            try {
                if (board[ky - 1 + i / 3][kx - 1 + i % 3].equals("k")) {
                    board=boardTemp;
                    return false;
                }
            } catch (Exception e) {
            }
        }
        
        //Checking for knight
        try {
            if (board[ky - 1][kx - 2].equals("n")) {
                board = boardTemp;
                return false;
            }
        } catch (Exception e) {}
        try {
            if (board[ky - 1][kx + 2].equals("n")) {
                board = boardTemp;
                return false;
            }
        } catch (Exception e) {}
        try {
            if (board[ky + 1][kx - 2].equals("n")) {
                board = boardTemp;
                return false;
            }
        } catch (Exception e) {}
        try {
            if (board[ky + 1][kx + 2].equals("n")) {
                board = boardTemp;
                return false;
            }
        } catch (Exception e) {}
        try {
            if (board[ky + 2][kx + 1].equals("n")) {
                board = boardTemp;
                return false;
            }
        } catch (Exception e) {}
        try {
            if (board[ky + 2][kx - 1].equals("n")) {
                board = boardTemp;
                return false;
            }
        } catch (Exception e) {}
        try {
            if (board[ky - 2][kx + 1].equals("n")) {
                board = boardTemp;
                return false;
            }
        } catch (Exception e) {}
        try {
            if (board[ky - 2][kx - 1].equals("n")) {
                board = boardTemp;
                return false;
            }
        } catch (Exception e) {}
        
        //Checking for pawn
        try {
            if (board[ky - 1][kx - 1].equals("p") || board[ky - 1][kx + 1].equals("p")) {
                board=boardTemp;
                return false;
            }
        } catch (Exception e) {
        }
        board=boardTemp;
        return true;
    }
    public static int KINGL(String[][] b){
        int hi = 0;
        for(int i=0;i<64;i++){
            if("K".equals(board[i/8][i%8])){
                return i;
            }
        }
        return hi;
    }
    public static int kingl(String[][] b){
        int hi = 0;
        for(int i=0;i<64;i++){
            if("k".equals(board[i/8][i%8])){
                return i;
            }
        }
        return hi;
    }
    public static void stalemate(){
        String pieceList="";
        for(int i=0;i<64;i++){
            if("Q".equals(board[i/8][i%8])||"q".equals(board[i/8][i%8])){//Stalemate not possible with a queen on whatever side
                return;
            }
            if("R".equals(board[i/8][i%8])||"r".equals(board[i/8][i%8])){//Stalemate not possible with a queen on whatever side
                return;
            }
            if("P".equals(board[i/8][i%8])||"p".equals(board[i/8][i%8])){//Stalemate not possible with a pawn on whatever side
                return;
            }
            if(!" ".equals(board[i/8][i%8])){
                pieceList+=board[i/8][i%8];
            }
        }
        if("Kk".equals(pieceList)||"kK".equals(pieceList)){//Two kings is a stalemate
            try {end("Stalemate!");}catch (IOException e){}
        }
        if("BKk".equals(pieceList)||"BkK".equals(pieceList)||"kBK".equals(pieceList)||"KBk".equals(pieceList)||"kKB".equals(pieceList)||"KkB".equals(pieceList)){
            try {end("Stalemate!");}catch (IOException e){}//2 kings + white bishop
        }
        if("bKk".equals(pieceList)||"bkK".equals(pieceList)||"kbK".equals(pieceList)||"Kbk".equals(pieceList)||"kKb".equals(pieceList)||"Kkb".equals(pieceList)){
            try {end("Stalemate!");}catch (IOException e){}//2 kings + black bishop
        }
        if("NKk".equals(pieceList)||"NkK".equals(pieceList)||"kNK".equals(pieceList)||"KNk".equals(pieceList)||"kKN".equals(pieceList)||"KkN".equals(pieceList)){
            try {end("Stalemate!");}catch (IOException e){}//2 kings + white knight
        }
        if("nKk".equals(pieceList)||"nkK".equals(pieceList)||"knK".equals(pieceList)||"Knk".equals(pieceList)||"kKn".equals(pieceList)||"Kkn".equals(pieceList)){
            try {end("Stalemate!");}catch (IOException e){}//2 kings + black knight
        }
        if("NNkK".equals(pieceList)||"NkNK".equals(pieceList)||"NkKN".equals(pieceList)||"kNKN".equals(pieceList)||"kKNN".equals(pieceList)||"NNKk".equals(pieceList)||"NKNk".equals(pieceList)||"NKkN".equals(pieceList)||"KNkN".equals(pieceList)||"KkNN".equals(pieceList)){
            try {end("Stalemate!");}catch (IOException e){}//2 kings + 2 white knights
        }
        if("NNkK".equals(pieceList)||"NkNK".equals(pieceList)||"NkKN".equals(pieceList)||"kNKN".equals(pieceList)||"kKNN".equals(pieceList)||"NNKk".equals(pieceList)||"NKNk".equals(pieceList)||"NKkN".equals(pieceList)||"KNkN".equals(pieceList)||"KkNN".equals(pieceList)){
            try {end("Stalemate!");}catch (IOException e){}//2 kings + 2 black knights
        }
    }
    public static void doTheSwap(){
        for(int i=0;i<64;i++){
            if(Character.isUpperCase(board[i/8][i%8].charAt(0))){
                board[i/8][i%8]=board[i/8][i%8].toLowerCase();
            }else if(Character.isLowerCase(board[i/8][i%8].charAt(0))){
                board[i/8][i%8]=board[i/8][i%8].toUpperCase();
            }
        }
        
        String[][] newBoard={
        {" "," "," "," "," "," "," "," "},
        {" "," "," "," "," "," "," "," "},
        {" "," "," "," "," "," "," "," "},
        {" "," "," "," "," "," "," "," "},
        {" "," "," "," "," "," "," "," "},
        {" "," "," "," "," "," "," "," "},
        {" "," "," "," "," "," "," "," "},
        {" "," "," "," "," "," "," "," "}};
        for(int i=0,j=63;i<64;i++,j--){
            /*
            if(i/8<=3){
                newBoard[i/8][i%8]=board[7-i/8][i%8];
            }
            if(i/8>=4){
                newBoard[i/8][i%8]=board[0+(7-i/8)][i%8];
            }
            */
            newBoard[i/8][i%8]=board[j/8][j%8];
        }
        board=newBoard;
    }
    public static String listMoves() {
        String moveList="";
        //Check each space to see if it has a white piece
        for(int i=0;i<64;i++){
            if(Character.isUpperCase(board[i/8][i%8].charAt(0/*<3*/))){
                
                //If it has a white piece, list all moves that the piece could possibly make, not checking for legality.
                for(int j=0;j<64;j++){
                    //Pieces cannot move onto themseleves
                    if(i!=j){
                        moveList+="(";
                        if(j>=0&&j<=7&&board[i/8][i%8].equals("P")){
                            moveList+="^";
                        }else{
                            moveList+=board[i/8][i%8];
                        }
                        moveList+=numToString(i);
                        moveList+=numToString(j);
                        moveList+=board[j/8][j%8];
                        moveList+=")";
                    }
                }
            }
        }
        //Putting all of these moves which are legal into another list
        //Lol this looks complicated
        String sortedMoveList="";
        for(int i=0;i<moveList.length()/8;i++){
            int n=Integer.parseInt(moveList.substring(i*8+2,i*8+4));
            int o=Integer.parseInt(moveList.substring(i*8+4,i*8+6));
            if(LEGAL(moveList.substring(i*8+1,i*8+2),n,o)){
                sortedMoveList+="("+moveList.substring(i*8+1,i*8+2);
                if(1==(int) Math.log10(n) + 1){
                    sortedMoveList+="0"+n;
                }else if(n==0){
                    sortedMoveList+="00";
                }else{
                    sortedMoveList+=n;
                }
                if(1==(int) Math.log10(o) + 1){
                    sortedMoveList+="0"+o;
                }else if(o==0){
                    sortedMoveList+="00";
                }else{
                    sortedMoveList+=o;
                }
                sortedMoveList+=moveList.substring(i*8+6,i*8+7)+")";
            }
        }
        return sortedMoveList;
    }
    public static String alphaBeta(int depth, int beta, int alpha, String move, int player) {//0 is the player. 1 is the com
        turn=player;
        aBCounter++;
        String list=com.listMoves();
        if (depth==0 || list.length()==0) {
            return move+(Rate.rate(list.length()/8,depth)*(player*2-1));
        }
        //sort later
        player=1-player;//either 1 or 0
        for (int i=0;i<list.length();i+=8) {
            
            move(list.substring(i,i+8));
            
            doTheSwap();
            String returnString=alphaBeta(depth-1, beta, alpha, list.substring(i,i+8), player);
            int value=Integer.valueOf(returnString.substring(8));
            doTheSwap();
            
            undo(list.substring(i,i+8));
            
            if (player==0) {
                if (value<=beta) {
                beta=value;
                if (depth==publicDepth) {
                    move=returnString.substring(0,8);
                }
            }
            } else {
                if (value>alpha) {
                    alpha=value;
                    if (depth==publicDepth) {
                        move=returnString.substring(0,8);
                    }
                }
            }
            if (alpha>=beta) {
                if (player==0) {
                    return move+beta;
                } else {
                    return move+alpha;
                }
            }
        }
        if (player==0) {
            return move+beta;
        } else {
            return move+alpha;
        }
    }
    public static void move(String m){
        board[Integer.parseInt(m.substring(2,4))/8][Integer.parseInt(m.substring(2,4))%8]=" ";
        
        //Promote pawn if ^
        if(m.charAt(1)=='^'){
            board[Integer.parseInt(m.substring(4,6))/8][Integer.parseInt(m.substring(4,6))%8]="Q";
        }else if(m.toUpperCase().charAt(1)!='C'){//If not castling
            board[Integer.parseInt(m.substring(4,6))/8][Integer.parseInt(m.substring(4,6))%8]=m.substring(1, 2);
        }else{//If castling
            if(Integer.parseInt(m.substring(2,4))==60){//Player castling
                if (m.charAt(1) == 'C') {//Kingside 
                    board[7][7] = " ";
                    board[7][6] = "K";
                    board[7][5] = "R";
                } else {//Queenside
                    board[7][0] = " ";
                    board[7][2] = "K";
                    board[7][3] = "R";
                }
            }else{//Computer castling
                if (m.charAt(1) == 'C') {//Kingside 
                    board[7][0] = " ";
                    board[7][1] = "K";
                    board[7][2] = "R";
                } else {//Queenside
                    board[7][7] = " ";
                    board[7][5] = "K";
                    board[7][4] = "R";
                }
            }
        }   
    }
    public static void undo(String m){
        //This is the same as move but with things swapped around
        board[Integer.parseInt(m.substring(4,6))/8][Integer.parseInt(m.substring(4,6))%8]=m.substring(6, 7);
        
        //Un-promote pawn if ^
        if(m.charAt(1)=='^'){
            board[Integer.parseInt(m.substring(2,4))/8][Integer.parseInt(m.substring(2,4))%8]="P";
        }else if(m.toUpperCase().charAt(1)!='C'){//If not castling
            board[Integer.parseInt(m.substring(2,4))/8][Integer.parseInt(m.substring(2,4))%8]=m.substring(1, 2);
        }else{//If castling
            if(m.charAt(1)=='C'){//Kingside
                board[7][7]="R";
                board[7][6]=" ";
                board[7][5]=" ";
                board[7][4]="K";
            }else{//Queenside
                board[7][0]="R";
                board[7][2]=" ";
                board[7][3]=" ";
                board[7][4]="K";
            }
        }
    }
    public static void checkEnd(String s) throws IOException{
        int kl=KINGL(board);
        boolean sk=com.safeK();
        if(sk){
            System.out.println("Stalemate!");
            doTheSwap();
            end("Stalemate!");
        }else{
            System.out.println(s);
            doTheSwap();
            end(s);
        }
    }
    public static void end(String s) throws IOException{
        InOut.log(s);
        JFrame f=new JFrame(s);
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        ChessInterface ui = new ChessInterface();
        f.add(ui);
        f.setSize(696, 719);
        f.setVisible(true);
    }
    public static void log(boolean W, String s){
        /*
        String o=numToString(63-Integer.parseInt(s.substring(2,4)));
        String n=numToString(63-Integer.parseInt(s.substring(4,6)));
        if(W){
            System.out.println(s);
        }else{
            System.out.print(s+" / ");
            System.out.print("("+s.substring(1,2).toLowerCase()+o);
            System.out.println(n+s.substring(6,7).toUpperCase()+")");
        }
        */
    }
}
