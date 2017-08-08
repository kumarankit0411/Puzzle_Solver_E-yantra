# Puzzle_Solver_E-yantra

## Note: This code runs only on FIREBIRD V Robot 

FOLLOWING ARE THE STEPS WE FOLLOWED FOR SUCCESSFUL RUNNING OF THE CODE:-


1. OPEN EMBEDDED-C CODE AND LOAD THE HEX FILE NAMED "black_line_follower.hex" IN THE DEFAULTS FOLDER 
  ONTO THE ROBOT.

2. ONCE LOADED TURN OFF AND ON THE ROBOT.

3. NOW WHILE THE SERIAL CABLE IS CONNECTED RUN "image_processing.py" FROM PYTHON CODE FOLDER.

"*NOTE:- IMAGE CAN BE CHANGED IN MAIN FUNCTION OF THE PYTHON CODE"

4. DURING THE RUN OF PYTHON CODE,OUTPUT IS DISPLAYED AND LISTS ARE TRANSFERRED TO THE BOT 
   SERIALLY  VIA "COM2" PORT IN OUR CASE.(PYSERIAL MODULE IS USED)

5. NOW REMOVE CABLE AND PRESS THE BOOT KEY WHILST THE ROBOT IS KEPT ON STARTING BLACK LINE.
     (CASTOR WHEEL IS ON THE THICK  START LINE)

6. ROBOT SHOULD SOLVE THE PUZZLE NOW.
