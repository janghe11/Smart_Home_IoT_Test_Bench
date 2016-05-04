<?php
    $errorCheck = 0;
    $andModeGet = null; // 1 -> Status check mode, 2 -> AVR control mode
    /* ====================andComm data variables====================
    * 'u' : Doorlock unlock 
    * 'l' : Doorlock lock
    * 't' : Boiler temp increase
    * 'b' : Boiler temp decrease
    * 'g' : Loosen gas valve
    * 'v' : Fasten gas valve
    */
    $andComm = null; 

    $avrDevId = "/dev/ttyUSB0"; // Device location of AVR USB
    $avrBoilerTemp = null;

    function rpiAndCommCheck($andComm, $avrBoilerTemp, $errorCheck)
    {
        switch($andComm) {
            case 'u':
                break;
            case 'l':
                break;
            case 't':
                if(($avrBoilerTemp < 18) || ($avrBoilerTemp > 30)) {
                    echo("Couldn't receive boiler temperature from android.\n");
                    $andComm = null;
                    $avrBoilerTemp = null;
                    $errorCheck = 1;
                }
                break;
            case 'b':
                if(($avrBoilerTemp < 18) || ($avrBoilerTemp > 30)) {
                    echo("Couldn't receive boiler temperature from android.\n");
                    $andComm = null;   
                    $avrBoilerTemp = null;
                    $errorCheck = 1;
                }
                break;
            case 'g':
                break;
            case 'v':
                break;
            default:
                echo("Android commnad token to AVR error.\n");
                $andComm = null;
                $errorCheck = 1;
        }
    }

    function avrSetComm($andComm, $avrBoilerTemp, $avrDevId)
    {
        $avrSendComm = "echo $andComm >> $avrDevId"; // Make command to AVR
        exec($avrSendComm); // Send data to AVR
        
        if($avrBoilerTemp != null)
            $avrSendComm = "echo $avrBoilerTemp >> $avrDevId"; // Make boiler temperature command to AVR
        exec($avrSendComm); // Send data to AVR
    }

    // Start Main
    if($andModeGet != null) { // Standby until receive data from android
        
        if($andModeGet == 1) {
            
        } else if($andModeGet == 2) { // Android to AVR command setter
            rpiAndCommCheck($andComm);
            
            if($andComm != null)
                avrSetComm($andComm, $avrDevId);    
            
            $andComm = null; // Reset Android command
        } else {
            echo("Android Mode Receive error! \n");
        }
        
        $andModeGet = null; // Reset Android Mode
    }
?>
