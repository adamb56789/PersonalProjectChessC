import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;


public class ChessInterface extends JPanel implements MouseListener, MouseMotionListener{
    
    //Declaring variables
    
    public static int square=85, sx=-100, sy=-100, oldX, oldY, bs1x=-100, bs1y=-100, bs2x=-100, bs2y=-100;
    int x, y;
    static boolean var0 = true, var1 = true, var2 = true;
    public static String  oldPiece;
   
    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent (g);
        this.setBackground(Color.LIGHT_GRAY);
        if (var2){
            this.addMouseListener(this);
            this.addMouseMotionListener(this);
            var2 = false;
        }

        //Drawing board
        
        g.setFont(new Font("TimesRoman", Font.PLAIN, 140));
        for (int i=0;i<64;i+=2) {
            g.setColor(Color.WHITE);
            g.fillRect((i%8+(i/8)%2)*square, (i/8)*square, square, square);
            g.setColor(Color.GRAY);
            g.fillRect(((i+1)%8-((i+1)/8)%2)*square, ((i+1)/8)*square, square, square);
        }
        
        //Importing images
        
        Image KING;
        KING = new ImageIcon("KING.png").getImage();
        
        Image QUEEN;
        QUEEN = new ImageIcon("QUEEN.png").getImage();
        
        Image ROOK;
        ROOK = new ImageIcon("ROOK.png").getImage();
        
        Image KNIGHT;
        KNIGHT = new ImageIcon("KNIGHT.png").getImage();
        
        Image BISHOP;
        BISHOP = new ImageIcon("BISHOP.png").getImage();
        
        Image PAWN;
        PAWN = new ImageIcon("PAWN.png").getImage();
        
        Image king;
        king = new ImageIcon("king..png").getImage();
        
        Image queen;
        queen = new ImageIcon("queen..png").getImage();
        
        Image rook;
        rook = new ImageIcon("rook..png").getImage();
        
        Image knight;
        knight = new ImageIcon("knight..png").getImage();
        
        Image bishop;
        bishop = new ImageIcon("bishop..png").getImage();
        
        Image pawn;
        pawn = new ImageIcon("pawn..png").getImage();
        
        Image selected;
        selected = new ImageIcon("selected.png").getImage();
        
        Image Bselected = new ImageIcon("blue_selected.png").getImage();

        //Drawing board based on board array
        
        for (int i=0;i<64;i++) {
            switch (Chess.board[i/8][i%8]) {
                case "P": g.drawImage(PAWN, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "p": g.drawImage(pawn, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "R": g.drawImage(ROOK, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "r": g.drawImage(rook, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "N": g.drawImage(KNIGHT, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "n": g.drawImage(knight, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "B": g.drawImage(BISHOP, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "b": g.drawImage(bishop, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "Q": g.drawImage(QUEEN, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "q": g.drawImage(queen, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "K": g.drawImage(KING, (i%8) * square - 3, (i/8) * square, this);
                    break;
                case "k": g.drawImage(king, (i%8) * square - 3, (i/8) * square, this);
                    break;
            }
        }
        //Show selected piece
        g.drawImage(selected, sx, sy, this);
        
        //Show computer moves
        g.drawImage(Bselected, bs1x, bs1y, this);
        g.drawImage(Bselected, bs2x, bs2y, this);
    }
    
    //Mouse Input
    
    @Override
    public void mouseMoved (MouseEvent e){
        //Where is the mouse?
        x = e.getX();
        y = e.getY();
    }
    @Override
    public void mouseClicked (MouseEvent e){  
    }
    @Override
    public void mouseReleased (MouseEvent e){
    }
    @Override
    public void mousePressed (MouseEvent e){
        if(Chess.MOVE){
            if (var1/*The first time you click*/){
                //Place border around selected square
                sx = ((x - x%square) / square) * square;
                sy = ((y - y%square) / square) * square;
            
                //Remember where the selected sqaure is
                oldX = x/square;
                oldY = y/square;
            
                //Remember what the selected piece is
                oldPiece = Chess.board[y/square][x/square];
            
                var1 = false;
            }else/*The second time you click*/{
                if (Chess.LEGAL(oldPiece, oldY*8 + oldX, y/square*8+x/square%8)){
                    String move="("+oldPiece+Chess.numToString(oldY*8 + oldX)+Chess.numToString(y/square*8+x/square%8)+Chess.board[y/square][x/square]+")";
                    
                    //Castling
                    if("K".equals(oldPiece)&&oldX+oldY*8==60){
                        if(y/square*8+x/square%8==62){//Kingside
                            move="(C6062 )";
                        }else if(y/square*8+x/square%8==58){//Queenside
                            move="(c6058 )";
                        }
                    }
                    try {
                        InOut.log(move);
                    } catch (IOException ex) {}
                    Chess.move(move);
                    
                    //Promote a PAWN if it is at the end of the board
                    Chess.PROMOTE();
                    
                    //Move the border off the window
                    sx = -100;
                    sy = -100;
                
                    //The computers turn
                    Chess.MOVE = false;
                    repaint();
                    String m="";
                    try {
                        m=Chess.myTurn();
                    } catch (IOException ex) {
                        Logger.getLogger(ChessInterface.class.getName()).log(Level.SEVERE, null, ex);
                    }
                    var1 = true;
                    BS(m);
                    repaint();
                }else{
                    sx = -100;
                    sy = -100;
                    var1 = true;
                }
            
            }
            repaint();
        }
    }
    @Override
    public void mouseDragged (MouseEvent e){
    }
    @Override
    public void mouseEntered (MouseEvent e){
    }
    @Override
    public void mouseExited (MouseEvent e){}

    public static void BS(String m) {
        bs1x=((63-Integer.parseInt(m.substring(2,4)))%8)*square;
        bs1y=((63-Integer.parseInt(m.substring(2,4)))/8)*square;
        bs2x=((63-Integer.parseInt(m.substring(4,6)))%8)*square;
        bs2y=((63-Integer.parseInt(m.substring(4,6)))/8)*square;
        System.out.println(bs1x);
        System.out.println(bs1y);
        System.out.println(bs2x);
        System.out.println(bs2y);
    }
}
