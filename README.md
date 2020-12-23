# FixMyAccordion
Fix Accordion left hand button mechanism

 
A few years ago a friend offered me an old and damaged accordion, someone
would have disassembled the left hand button panel mechanism
and  was no longer able to assemble all the parts as original.
As a result, it was impossible to play a song on that instrument.
This was the starting point and the impetus for carrying out this work.


Description:

        8 rows of buttons: MI, LA, RE, SOL, DO, FA, SIb, MIb.
        4 lines of buttons: Bass, Major, Minor, Seventh.
        12 keys with 3 or 4 pins that will activate 3 or 4 shafts.
        12 shafts with 2, 3 or 4 pins that drive 1 valve.
        
        For simplification, we will separate the keys with 4 pins, which are the keys of the seventh chord.
     Comparing these with each of the others, we will separate the ones that match 3 pins, they are the keys 
     of the Major chord. The rest that differ only one pin are the Minor chord.
        We now have 3 groups of keys: Major, Minor and Seventh.
     Now we just need to identify the name and position of the 8 keys of the Major group, and then by comparison
     we will be able to identify and assemble the rest.

     To represent the shafts, let's see how many parts we can divide a shaft, that is, how many positions there 
     may be pins.
     By placing a shaft arbitrarily in one of the positions of the structure of the panel, we can verify that 
     there may be pins in any of the spaces that separate the grooves where the keys work, and there are 25 
     spaces. Thus, a DWORD can represent a shaft with bits 1 at the pin position.
    
                     1       8            20
                    _|_______|____________|_____
                         25 units in length
    
    
    ShaftList[][]=
    {
    ...
        {1, 8, 20, 0},   // some of shafts have 4 pins
    ...    
    };


    TShaft::TShaft(int n1, int n2, int n3, int n4)
    {
        conf = 0; name[0] = '\0'; status = 0;
        if(n1)conf = (DWORD)1 << (n1 - 1);
        if(n2)conf |= (DWORD)1 << (n2 - 1);
        if(n3)conf |= (DWORD)1 << (n3 - 1);
        if(n4)conf |= (DWORD)1 << (n4 - 1);
    }

    TShaft * tr = new TShaft(ShaftList[n][0], ShaftList[n][1], ShaftList[n][2], ShaftList[n][3]); 

To represent the keys, they can be divided into 12 positions, corresponding to each of the shafts, 
and differentiating the sides with negative and positive numbers.


                               9
                       _________|___
                            |      |
                            -5    -12
                            
        static int MajorKeys[8][3] ={
            {-5, 9, -12},
            ...
        }

        void TKey::MakeConfig(int n1, int n2, int n3, int n4)
        {
              Lconf = Rconf = 0;
              if (n1 > 0)      Rconf |= (DWORD)1 << (n1 - 1);
              else if (n1 < 0) Lconf |= (DWORD)1 << (abs(n1) - 1);
              if (n2 > 0)      Rconf |= (DWORD)1 << (n2 - 1);
              else if (n2 < 0) Lconf |= (DWORD)1 << (abs(n2) - 1);
              if (n3 > 0)      Rconf |= (DWORD)1 << (n3 - 1);
              else if (n3 < 0) Lconf |= (DWORD)1 << (abs(n3) - 1);
              if (n4 > 0)      Rconf |= (DWORD)1 << (n4 - 1);
              else if (n4 < 0) Lconf |= (DWORD)1 << (abs(n4) - 1);
        }

        TKey *k = new TKey(MajorKeys[index[n]][0], MajorKeys[index[n]][1], MajorKeys[index[n]][2]);
 


Now that we have the 8 keys and the 12 shafts represented, we need to arrange all this in the correct order, 
so that when we press, or in this case, when we click with the mouse on one of the keys, they activate the 
shafts that open the valves to the desired chord.
For this, it is essential to know that <The third note of a chord is the first of the next chord>, for example 
the third of the DO chord is SOL (DO, MI, SOL), the next chord will be SOL. The third of SOL is RE (SOL, SI, RE)
the next chord will be RE, etc.etc. .
Now our list of keys after being sorted in the Find method has been modified, now containing all the keys in 
the correct sequence.
    
    
           
     static int TMaior[8][3]            index[8][3]
     {
        {-5,9,-12},       ->            {-12,4,-7},   // MI
        {-11,3,-6},       ->            {-5,9,-12},   // LA
        {-6,10,-1},       ->            {-10,2,-5},   // RE
        {-1,5,-8},        ->            {-3,7,-10},   // SOL
        {-8,12,-3},       ->            {-8,12,-3},   // DO
        {-3,7,-10},       ->            {-1,5,-8},    // FA
        {-10,2,-5},       ->            {-6,10,-1},   // SIb
        {-12,4,-7}        ->            {-11,3,-6}    // MIb
    };



 
     Enjoy and have fun.
