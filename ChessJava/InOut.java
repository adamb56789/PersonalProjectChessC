import java.io.*;

public class InOut {
    static boolean player=true;
    static String turn;
    static int turnN=1;
    public static void write(String s) throws IOException{
        try(PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(Chess.startTime, true)))) {
            out.println(s);
        }catch (IOException e) {}
    }
    public static void log(String s)throws IOException{
        System.out.println(s);
        if (!"Checkmate - You Lost!".equals(s)&&!"Stalemate".equals(s)&&!"Checkmate - You Won!".equals(s)) {
            s=s.substring(0,8);
        }
        try {
            if (!player) {//Swap locations
                int o = Integer.parseInt(s.substring(2, 4)), n = Integer.parseInt(s.substring(4, 6));
                o = 63 - o;
                n = 63 - n;
                String ns = "(" + s.substring(1, 2).toLowerCase();
                if (1 == (int) Math.log10(o) + 1) {
                    ns += "0" + o;
                } else if (o == 0) {
                    ns += "00";
                } else {
                    ns += o;
                }
                if (1 == (int) Math.log10(n) + 1) {
                    ns += "0" + n;
                } else if (n == 0) {
                    ns += "00";
                } else {
                    ns += n;
                }
                ns += s.substring(6, 7) + ")";
                s = ns;
            }
            
        } catch (NumberFormatException numberFormatException) {}
        //Convert to proper notation
        if(s.charAt(0)=='('){s=convert(s);}
        
        //Build the line
        if (player) {
            turn = turnN + ". " + s + " ";
            player = false;
        } else {
            turn += s;
            write(turn);
            player = true;
            turnN++;
        }
        
        //Stalemate
        if("Stalemate".equals(s)){
            write("½–½");
        }
        //Checkmate
        System.out.println(s);
        if("Checkmate - You Lost!".equals(s)){
            write("0-1");System.out.println("YAY");
        }
        if("Checkmate - You Won!".equals(s)){
            write("1-0");
        }
    }
    public static String convert(String s){
        String move="";
        
        try {
            //Piece
            if (s.charAt(1) != 'P' && s.charAt(1) != 'c' && s.charAt(1) != 'C' && s.charAt(1) != '^' && s.charAt(1) != 'p') {
                move += s.charAt(1);
                switch (Integer.parseInt(s.substring(2, 4)) % 8) {
                    case 0:
                        move += "a";
                        break;
                    case 1:
                        move += "b";
                        break;
                    case 2:
                        move += "c";
                        break;
                    case 3:
                        move += "d";
                        break;
                    case 4:
                        move += "e";
                        break;
                    case 5:
                        move += "f";
                        break;
                    case 6:
                        move += "g";
                        break;
                    case 7:
                        move += "h";
                        break;
                }

                //y coordinate to swapped number from 1-8
                move += 8 - (Integer.parseInt(s.substring(2, 4)) / 8) + "";
            }

            //Add x for captures
            if (s.charAt(6) != ' ') {
                move += "x";
            }

            //x coordinate to letter
            switch (Integer.parseInt(s.substring(4, 6)) % 8) {
                case 0:
                    move += "a";
                    break;
                case 1:
                    move += "b";
                    break;
                case 2:
                    move += "c";
                    break;
                case 3:
                    move += "d";
                    break;
                case 4:
                    move += "e";
                    break;
                case 5:
                    move += "f";
                    break;
                case 6:
                    move += "g";
                    break;
                case 7:
                    move += "h";
                    break;
            }

            //y coordinate to swapped number from 1-8
            move += 8 - (Integer.parseInt(s.substring(4, 6)) / 8) + "";

            //If promotion
            if (s.charAt(1) == '^') {
                move += "=Q";
            }

            //Check
            Chess.doTheSwap();
            if (!com.safeK()) {
                move += "+";
            }
            Chess.doTheSwap();

            //Castling
            if (s.charAt(1) == 'C') {//Kingside
                move = "0-0";
            }
            if (s.charAt(1) == 'c') {//Queenside
                move = "0-0-0";
            }
            
        } catch (NumberFormatException numberFormatException) {}
        
        return move;
    }
    public static String[] settings(){
        String[] settings= new String[2];//Create array with two elements
        String name="Settings.txt";
        
        try{
            FileReader fR = new FileReader(name);
            try (BufferedReader bR = new BufferedReader(fR)) {
                settings[0] = bR.readLine();
                settings[1] = bR.readLine();
            }
        }catch(Exception e){}
        return settings;
    }
}
