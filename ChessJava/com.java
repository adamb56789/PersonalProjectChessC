public class com {
    public static String listMoves(){
        String list="";
        for(int i=0;i<64;i++){
            switch(Chess.board[i/8][i%8]){
                case "P": list+=lP(i%8,i/8);
                    break;
                case "N": list+=lN(i%8,i/8);
                    break;
                case "B": list+=lB(i%8,i/8);
                    break;
                case "R": list+=lR(i%8,i/8);
                    break;
                case "Q": list+=lQ(i%8,i/8);
                    break;
                case "K": list+=lK(i%8,i/8);
                    break;
            }
        }
        return list;
    }
    public static String lP(int x,int y){
        String list="";
        //Move one forward
        if(Chess.board[y-1][x].equals(" ")){
            if(Chess.safeK(x, y, x, y-1)){
                list+=myNotation(x,y,x,y-1);
            }
            //Move two forward
            if (y==(6)) {
                try {
                    if (Chess.board[y - 2][x].equals(" ")) {
                        if (Chess.safeK(x, y, x, y - 2)) {
                            list += myNotation(x, y, x, y - 2);
                        }
                    }
                } catch (Exception e) {
                }
            }
        }
        
        //Diagonal move (taking)
        for (int i=-1;i <=1;i+=2) {
            try {
                if (Character.isLowerCase(Chess.board[y - 1][x + i].charAt(0))) {
                    if (Chess.safeK(x, y, x + i, y - 1)) {
                        list += myNotation(x, y, x+i, y - 1);
                    }
                }
            } catch (Exception e) {}
        }
        return list;
    }
    public static String lN(int x,int y){
        String list="";
        for(int i=-1;i<=1;i+=2){
            for(int j=-1;j<=1;j+=2){
                try {
                    if (!Character.isUpperCase(Chess.board[y + i][x + j * 2].charAt(0))) {
                        if (Chess.safeK(x, y, x + j * 2, y + i)) {
                            list += myNotation(x, y, x + j * 2, y + i);
                        }
                    }
                } catch (Exception e) {}
                try {
                    if (!Character.isUpperCase(Chess.board[y + i * 2][x + j].charAt(0))) {
                        if (Chess.safeK(x, y, x + j, y + i * 2)) {
                            list += myNotation(x, y, x + j, y + i * 2);
                        }
                    }
                } catch (Exception e) {}
            }
        }
        return list;
    }
    public static String lB(int x,int y){
        String list="";
        boolean k = true;
        int t=1;
        
        //Check for each direction of movement
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                //Only allow if diagonal
                if (i != 0 && j != 0) {
                    while (k) {
                        int ny = y + i * t;
                        int nx = x + j * t;
                        t++;
                        try {
                            if (Chess.board[ny][nx].equals(" ")) {
                                if (Chess.safeK(x, y, nx, ny)) {
                                    list += myNotation(x, y, nx, ny);
                                }
                            } else if (Character.isLowerCase(Chess.board[ny][nx].charAt(0))) {
                                k = false;
                                if (Chess.safeK(x, y, nx, ny)) {
                                    list += myNotation(x, y, nx, ny);
                                }
                            } else {
                                k = false;
                            }
                        } catch (Exception exception) {
                            k = false;
                        }
                    }
                }
                k = true;
                t = 1;
            }
        }
        return list;
    }
    public static String lR(int x,int y){
        String list="";
        boolean k = true;
        int t=1;
        
        //Check for each direction of movement
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                //Only allow if in a straigt line
                if (i == 0 || j == 0) {
                    while (k) {
                        int ny = y + i * t;
                        int nx = x + j * t;
                        t++;
                        try {
                            if (Chess.board[ny][nx].equals(" ")) {
                                if (Chess.safeK(x, y, nx, ny)) {
                                    list += myNotation(x, y, nx, ny);
                                }
                            } else if (Character.isLowerCase(Chess.board[ny][nx].charAt(0))) {
                                k = false;
                                if (Chess.safeK(x, y, nx, ny)) {
                                    list += myNotation(x, y, nx, ny);
                                }
                            } else {
                                k = false;
                            }
                        } catch (Exception exception) {
                            k = false;
                        }
                    }
                }
                k = true;
                t = 1;
            }
        }
        return list;
    }
    public static String lQ(int x,int y){
        String list="";
        boolean k = true;
        int t=1;
        
        //Check for each direction of movement
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                while(k){
                    int ny = y+i*t;
                    int nx = x+j*t;
                    t++;
                    try{
                        if(Chess.board[ny][nx].equals(" ")){
                            if(Chess.safeK(x,y,nx,ny)){
                                list += myNotation(x, y, nx, ny);
                            }
                        }else if(Character.isLowerCase(Chess.board[ny][nx].charAt(0))){
                            k = false;
                            if(Chess.safeK(x,y,nx,ny)){
                                list += myNotation(x, y, nx, ny);
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
        return list;
    }
    public static String lK(int x,int y){
        String list="";
        for(int i=-1;i<=1;i++){
            for(int j=-1;j<=1;j++){
                try {
                    if (!Character.isUpperCase(Chess.board[y + i][x + j].charAt(0))) {
                        if (Chess.safeK(x, y, x + j, y + i)) {
                            list += myNotation(x, y, x + j, y + i);
                        }
                    }
                } catch (Exception e) {}
            }
        }
        //Castling
        if(Chess.turn==0){//Player castling
            if (x == 4 && y == 7) {
                if ("R".equals(Chess.board[7][7]) && " ".equals(Chess.board[7][6]) && " ".equals(Chess.board[7][5])) {
                    return "(C6063 )";//Kingside
                }
                if ("R".equals(Chess.board[7][0]) && " ".equals(Chess.board[7][1]) && " ".equals(Chess.board[7][2]) && " ".equals(Chess.board[7][3])) {
                    return "(c6058 )";//Queenside
                }
            }
        }
        return list;
    }
    public static boolean safeK(){
        int Kl = Chess.KINGL(Chess.board);
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
                        String b = Chess.board[cy][cx];
                        if(Character.isUpperCase(b.charAt(0))){
                            k=false;
                        }else if(Character.isLowerCase(b.charAt(0))){
                            k=false;
                            if(b.equals("q")){
                                return false;
                            }else if (b.equals("r")&&(i==0||j==0)){
                                return false;
                            }else if (b.equals("b")&&i!=0&&j!=0){
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
                if (Chess.board[ky - 1 + i / 3][kx - 1 + i % 3].equals("k")) {
                    return false;
                }
            } catch (Exception e) {
            }
        }
        
        //Checking for knight
        try {
            if (Chess.board[ky - 1][kx - 2].equals("n")) {
                return false;
            }
        } catch (Exception e) {}
        try {
            if (Chess.board[ky - 1][kx + 2].equals("n")) {
                return false;
            }
        } catch (Exception e) {}
        try {
            if (Chess.board[ky + 1][kx - 2].equals("n")) {
                return false;
            }
        } catch (Exception e) {}
        try {
            if (Chess.board[ky + 1][kx + 2].equals("n")) {
                return false;
            }
        } catch (Exception e) {}
        try {
            if (Chess.board[ky + 2][kx + 1].equals("n")) {
                return false;
            }
        } catch (Exception e) {}
        try {
            if (Chess.board[ky + 2][kx - 1].equals("n")) {
                return false;
            }
        } catch (Exception e) {}
        try {
            if (Chess.board[ky - 2][kx + 1].equals("n")) {
                return false;
            }
        } catch (Exception e) {}
        try {
            if (Chess.board[ky - 2][kx - 1].equals("n")) {
                return false;
            }
        } catch (Exception e) {}
        
        //Checking for pawn
        try {
            if (Chess.board[ky - 1][kx - 1].equals("p") || Chess.board[ky - 1][kx + 1].equals("p")) {
                return false;
            }
        } catch (Exception e) {
        }
        return true;
    }
    public static String myNotation(int ox,int oy,int nx,int ny){
        int o=oy*8+ox;
        int n=ny*8+nx;
        String move="("+Chess.board[o/8][o%8];
        if("P".equals(Chess.board[oy][ox])&&ny==0){
            move="(^";
        }
        if(1==(int) Math.log10(o) + 1){
            move+="0"+o;
        }else if(o==0){
            move+="00";
        }else{
            move+=o;
        }
        if(1==(int) Math.log10(n) + 1){
            move+="0"+n;
        }else if(n==0){
            move+="00";
        }else{
            move+=n;
        }
        move+=Chess.board[n/8][n%8]+")";
        return move;
    }
}
