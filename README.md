# Ethernet-Chat-App
Airties Summerseed Project


tranceiver.c receives packets and send them answer if necessary.

trans.c used for send packets to targets and broadcast queries.

//example:

    $gcc trans.c -o trans
    $sudo ./trans 0 //sends broadcast discover query
    
    $sudo ./trans 1 cihan alp  //sends unicast query
    
    $sudo ./trans 3 cihan alp heyyo im chatting with cihan alp  //sends message to cihan alp written after "cihan alp" argument
    
    
    
    
    
TODO LIST:

  When writing to file mac address check for file is empty before checking the mac is duplicate. 
    
