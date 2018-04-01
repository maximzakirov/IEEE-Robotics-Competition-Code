// Authors:  Haydn Brown, Ishmael Delen, Max Zakirov, David Song, Blayne Howard
// School: Earl of March
//
// April 29th - Started as Yom_Yom1 - Basic right turn code when an obstacle is detected. (Same as Line_Stop2.ino)
// April 30th - Yom_Yom3 - First major coding attempt.  This code can't currently
//              deal with turning into an obstacle which we will definitely need to deal with
//              (But for a plan on how to deal with this, see the notes in the black/black
//              section.
//  May 4th -   Yom_Yom8 - Added Outputs to drive LEDs for x and y co-ords.  I/O pin 2 (MSB) and 3 (LSB) specify x 
//              co-ord in binary. I/O pin 8 (MSB)and 9 (LSB) specify y co-ord in binary.   
//  May 5th -   Yom_Yom9 - Motor speed values set to 74/76 for Yom-Yom, but now made into constants, 
//              left_mot_fwd_speed and right_mot_fwd_speed, to make it easier to calibrate after transport.
//              Also added correct_mot_speed=100 for the correcting speed.  Slowed down the turns to 85 from 120
//              and increased the time it takes from 32000 to 45000
//              since things were a bit jerky and quick.  Noted that random function isn't working properly because
//              a random number of 1 or 2 should be specified with random(1,3).  Fixed this.
#include <IRControl.h>
#include <MotorControl.h>
#include <UltrasonicControl.h>

//Configure the sensor and motor objects
IRControl ir1(0);  //right reflectence sensor
IRControl ir2(1);   //left reflectence sensor
MotorControl mot1(1); //left motor
MotorControl mot2(2);  //right motor
UltrasonicControl sonic (13,12);  //ultrasonic sensor

//Set up constants to make tweeking values easier
const int UP=0, RIGHT=1, DOWN=2, LEFT=3;
const int left_mot_fwd_speed=74, right_mot_fwd_speed=76;
const int correct_mot_speed=100;
const int left_turn_speed=85, right_turn_speed=85;

int x=0, y=1, dir=UP; //Global variables, updated from evaluateTurn() function
   
void setup() 
{
   Serial.begin(9600);
   delay(5000);  //give us 5 seconds to place the robot
   randomSeed(analogRead(2));  //Want our random choices to be truly random
   pinMode(2, OUTPUT); //X co-ord MSB LED
   pinMode(3, OUTPUT); //X co-ord LSB LED
   pinMode(8, OUTPUT); //Y co-ord MSB LED
   pinMode(9, OUTPUT); //Y co-ord LSB LED
}

void loop() 
{  
  //Set up serial port to report reflectence sensor values
   Serial.print(ir1.isBlack());
   Serial.print("\t");
   Serial.print(ir2.isBlack());
   Serial.print("\t");
   Serial.println(sonic.getDistance());
  
   if (ir1.isBlack() == 0 && ir2.isBlack() == 0)  //white on both sides of sensors
   {
      mot1.forward(left_mot_fwd_speed);
      mot2.forward(right_mot_fwd_speed);
   }
  
   else if ((ir1.isBlack() == 1) && (ir2.isBlack() == 0)) //moving left
   {
      mot1.forward(correct_mot_speed);  //turn up left wheel to correct to the right
      mot2.forward(right_mot_fwd_speed);
   }

   else if ((ir1.isBlack() == 0) && (ir2.isBlack() == 1))//moving right
   {
      mot1.forward(left_mot_fwd_speed);  //turn up the right wheel to correct to the left
      mot2.forward(correct_mot_speed);
   }
  
   else if ((ir1.isBlack() == 1) && (ir2.isBlack() == 1))  //hit and intersection and stop!
   {
      stop(); //delay for a brief second to evaluate

      evaluateTurn();

      setLEDs();

      kickStart();  //Later on put this inside a sonic.detect() condition like:
                    //
                    //  if(!sonic.detect()){
                    //    kickStart();
                    //  }else{
                    //    evaluate Turn();
                    //    kickStart();
                    //  }
                    //   
                    //  Note: This may require a fine tuning of our turnRight
                    //        and turnLeft turns...we could make them much
                    //        more involved if necessary. 
                    //
                    //  Note2: Will also have to "un"-decrement the variables if
                    //         we go into the else portion above.  This may
                    //         require us to return the x and y coord so we
                    //         can modify x,y and dir accordingly later
 
   }
}

void setLEDs()
{
  //light up proper binary code for x co-ord
  
   if (x==0)
   {
     digitalWrite(2,HIGH);
     digitalWrite(3,HIGH);
   }
   else if (x==1)
   {
     digitalWrite(2,HIGH);
     digitalWrite(3,LOW);
   }
   else if (x==2)
   {
     digitalWrite(2,LOW);
     digitalWrite(3,HIGH);
   }
   else if (x==3)
   {
     digitalWrite(2,LOW);
     digitalWrite(3,LOW);
   }

   //light up proper binary code for y co-ord
   
   if (y==0)
   {
     digitalWrite(8,HIGH);
     digitalWrite(9,HIGH);
   }
   else if (y==1)
   {
     digitalWrite(8,HIGH);
     digitalWrite(9,LOW);
   }
   else if (y==2)
   {
     digitalWrite(8,LOW);
     digitalWrite(9,HIGH);
   }
   else if (y==3)
   {
     digitalWrite(8,LOW);
     digitalWrite(9,LOW);
   }
     
}

void stop()
{
   for (long i=0; i<10000 ; i++)  //Note: Needs to be long for 4 byte storage
   {                               //Rather than 2 byte storage of int
      mot1.halt();
      mot2.halt();
   }
}

void turnRight()   //Will need to adjust this to move both wheels for tighter turns
{
   for (long j=0; j<45000; j++)
   {
      mot1.forward(right_turn_speed);
      mot2.forward(0);
   }
}

void turnLeft(){    //Will need to adjust this to move both wheels for tighter turns
   for (long j=0; j<45000; j++)
   {
      mot1.forward(0);
      mot2.forward(left_turn_speed);
   }
}

void kickStart()   //Gets the robot moving again after a decision or a turn
{
   for (long k=0; k<20000; k++)
   {
      mot1.forward(left_mot_fwd_speed);
      mot2.forward(right_mot_fwd_speed);
   }
}

//evaluateTurn() - a function that decides the turn
//updating x,y and dir can be a separate function later on???

//After looking at the grid of 16 nodes, we identified 9 nodes
//that behave in similar ways.  9 nodes times 4 directions gives
//us 36 if statements upon where we can make decisions for turning.
//All decisions are made using the x and y position  and dir direction
//global variables.  This function doesn't return anything as the
//global variables are updated here after a decision is made.

//The beauty of this code is that by considering x,y position and
//direction to make decisions about turning we automatically build 
//in that the robot can never 
//go off of the end of the board and that it will always choose
//a move that gets it closer to its destination.

void evaluateTurn()
{
      int choice=0;

      //Go through all 36 possible cases
      
      if (x==0 && y==0 && dir==UP)
      {
        if (sonic.detect())
        {
          turnRight();
          dir=RIGHT;
          x++;
        }
        else
        {
          choice=random(1,3);
          if (choice == 1)
          {
            dir=UP;
            y++;
          }
          else
          {
            turnRight();
            dir=RIGHT;
            x++;
          }
        }
      }

      else if (x==0 && y==0 && dir==RIGHT)
      {
        if (sonic.detect())
        {
          turnLeft();
          dir=UP;
          y++;
        }
        else
        {
          choice=random(1,3);
          if (choice == 1)
          {
            dir=RIGHT;
            x++;
          }
          else
          {
            turnLeft();
            dir=UP;
            y++;
          }
        }
      }
      
      else if (x==0 && y==0 && dir==DOWN)
      {
        turnLeft();
        dir=RIGHT;
        x++;
      } 

      else if (x==0 && y==0 && dir==LEFT)
      {
        turnRight();
        dir=UP;
        y++;
      }

      else if (((x==0 && y==1) || (x==0 && y==2)) && dir==UP)
      {
        if (sonic.detect())
        {
          turnRight();
          dir=RIGHT;
          x++;
        }
        else
        {
          choice=random(1,3);
          if (choice == 1)
          {
            dir=UP;
            y++;
          }
          else
          {
            turnRight();
            dir=RIGHT;
            x++;
          }
        }
      }

      else if (((x==0 && y==1) || (x==0 && y==2)) && dir==RIGHT)
      {
        if (sonic.detect())
        {
          turnLeft();
          dir=UP;
          y++;
        }
        else
        {
          choice=random(1,3);
          if (choice == 1)
          {
            dir=RIGHT;
            x++;
          }
          else
          {
            turnLeft();
            dir=UP;
            y++;
          }
        }
      }

      else if (((x==0 && y==1) || (x==0 && y==2)) && dir==DOWN)
      {
        if (sonic.detect())
        {
          turnLeft();
          dir=RIGHT;
          x++;
        }
        else
        {
          turnLeft();
          dir=RIGHT;
          x++;
        }
      }

      else if (((x==0 && y==1) || (x==0 && y==2)) && dir==LEFT)
      {
        turnRight();
        dir=UP;
        y++;
      }

      else if (x==0 && y==3 && dir==UP){
        turnRight();
        dir=RIGHT;
        x++;
      }

      else if (x==0 && y==3 && dir==RIGHT)
      {
        if (sonic.detect())
        {
           turnRight();
           dir=DOWN;
           y--;
        }
        else
        {
          choice=random(1,3);
          if (choice==1)
          {
            dir=RIGHT;
            x++;
          }
          else
          {
            turnRight();
            dir=DOWN;
            y--;
          }
        }
      }

      else if (x==0 && y==3 && dir==DOWN)
      {
        if (sonic.detect())
        {
          turnLeft();
          dir=RIGHT;
          x++;
        }
        else
        {
          turnLeft();
          dir=RIGHT;
          x++;
        }
      }

      else if (x==0 && y==3 && dir==LEFT){
        turnLeft();
        dir=DOWN;
        y--;
      }

      else if (((x==1 && y==0)  || (x==2 && y==0)) && dir==UP)
      {
        if (sonic.detect())
        {
          turnRight();
          dir=RIGHT;
          x++;
        }
        else
        {
          choice=random(1,3);
          if (choice==1)
          {
            dir=UP;
            y++;
          }
          else
          {
            turnRight();
            dir=RIGHT;
            x++;
          }
        }
      }

      else if (((x==1 && y==0)  || (x==2 && y==0)) && dir==RIGHT)
      {
        if (sonic.detect())
        {
          turnLeft();
          dir=UP;
          y++;
        }
        else
        {
          choice=random(1,3);
          if (choice==1)
          {
            dir=RIGHT;
            x++;
          }
          else
          {
            turnLeft();
            dir=UP;
            y++;
          }
        }
      }

      else if (((x==1 && y==0)  || (x==2 && y==0)) && dir==DOWN)
      {
        turnLeft();
        dir=RIGHT;
        x++;
      }

      else if (((x==1 && y==0)  || (x==2 && y==0)) && dir==LEFT)  
      {
        if (sonic.detect())
        {
          turnRight();
          dir=UP;
          y++;    
        }
        else
        {
          turnRight();
          dir=UP;
          y++;
        }
      }

      else if (((x==1 && y==1)  || (x==2 && y==1) || (x==1 && y==2) || (x==2 && y==2)) && dir==UP)
      {
        if (sonic.detect())
        {
          turnRight();
          dir=RIGHT;
          x++;
        }
        else
        {
          choice=random(1,3);
          if (choice==1)
          {
            dir=UP;
            y++;
          }
          else
          {
            turnRight();
            dir=RIGHT;
            x++;
          }
        }
      }

      else if (((x==1 && y==1)  || (x==2 && y==1) || (x==1 && y==2) || (x==2 && y==2)) && dir==RIGHT)
      {
        if (sonic.detect())
        {
          turnLeft();
          dir=UP;
          y++;
        }
        else
        {
          choice=random(1,3);
          if (choice==1)
          {
            dir=RIGHT;
            x++;
          }
          else
          {
            turnLeft();
            dir=UP;
            y++;
          }
        }
      }

      else if (((x==1 && y==1)  || (x==2 && y==1) || (x==1 && y==2) || (x==2 && y==2)) && dir==DOWN)
      {
        if (sonic.detect())
        {
          turnLeft();
          dir=RIGHT;
          x++;
        }
        else
        {
          turnLeft();
          dir=RIGHT;
          x++;
        }
      }

      else if (((x==1 && y==1)  || (x==2 && y==1) || (x==1 && y==2) || (x==2 && y==2)) && dir==LEFT)
      {
        if (sonic.detect())
        {
          turnRight();
          dir=UP;
          y++;
        }
        else
        {
          turnRight();
          dir=UP;
          y++;
        }
      }

      else if (((x==1 && y==3) || (x==2 && y==3)) && (dir==UP))
      {
        turnRight();
        dir=RIGHT;
        x++;
      }

      else if (((x==1 && y==3) || (x==2 && y==3)) && (dir==RIGHT))
      {
        if(sonic.detect())
        {
          turnRight();
          dir=DOWN;
          y--;
        }
        else
        {
          dir=RIGHT;
          x++;
        }
      }

      else if (((x==1 && y==3) || (x==2 && y==3)) && (dir==DOWN))
      {
        if(sonic.detect())
        {
          turnLeft();
          dir=RIGHT;
          x++;
        }
      }

      else if (((x==1 && y==3) || (x==2 && y==3)) && (dir==LEFT))
      {
        if(sonic.detect())
        {
          turnLeft();
          dir=DOWN;
          y--;
        }
        else
        {
          turnLeft();
          dir=DOWN;
          y--;
        }
      }

      else if  (x==3 && y==0 && dir==UP)
      {
        if (sonic.detect())
        {
          turnLeft();
          dir=LEFT;
          x--;
        }
        else
        {
          dir=UP;
          y++;
        }
      }

      else if (x==3 && y==0 && dir==RIGHT)
      {
        turnLeft();
        dir=UP;
        y++;
      }

      else if (x==3 && y==0 && dir==DOWN)
      {
        turnRight();
        dir=LEFT;
        x--;
      }

      else if (x==3 && y==0 && dir==LEFT)
      {
        if (sonic.detect())
        {
          turnRight();
          dir=UP;
          y++;
        }
        else
        {
          turnRight();
          dir=UP;
          y++;
        }
      }

      else if (((x==3 && y==1) || (x==3 && y==2)) && dir==UP)
      {
        if (sonic.detect())
        {
          turnLeft();
          dir=LEFT;
          x--;
        }
        else
        {
          dir=UP;
          y++;
        }
      }

      else if (((x==3 && y==1) || (x==3 && y==2)) && dir==RIGHT)
      {
        turnLeft();
        dir=UP;
        y++;
      }

      else if (((x==3 && y==1) || (x==3 && y==2)) && dir==DOWN)
      {
        if (sonic.detect())
        {
          turnRight();
          dir=LEFT;
          x--;    
        }
        else
        {
          turnRight();
          dir=LEFT;
          x--;
        }
      }

      else if (((x==3 && y==1) || (x==3 && y==2)) && dir==LEFT)     
      {
        if (sonic.detect())
        {
          turnRight();
          dir=UP;
          y++; 
        }
        else
        {
          turnRight();
          dir=UP;
          y++;
        }
      }

      else if ((x==3 && y==3) && (dir==UP || dir==RIGHT))
      {
        //Victory Dance!!! 4 clockwise spins and then halt
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        turnRight();
        while(true)
        {
          stop();
        }
      }
      else
      {
        //Something has gone wrong - start spinning counterclockwise forever!?@#
        while(true)
        {
          turnLeft();
        }
      }
             
}



