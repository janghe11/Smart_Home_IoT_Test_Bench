<?php
    $errorCheck = 0;
    $andModeGet = 1; // 1 -> Status check mode, 2 -> AVR control mode

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

    function andGetMaria()
    {
        $mariaHost = 'localhost';
        $mariaUser = 'root';
        $mariaPassword = 'raspberry';
        $mariaDBName = 'and_rpi_avr';
        $mariaToJson = array();
        
        $mariaConnect = mysql_connect($mariaHost, $mariaUser, $mariaPassword) or die("DB Connect Error");
        $mariaDBConnect = mysql_select_db($mariaDBName, $mariaConnect);
        mysql_query("set names utf8");
        $mariaQuery = "select * from rpi_avr";
        $getMariaData = mysql_query($mariaQuery, $mariaConnect);
            
        while($mariaRow = mysql_fetch_array($getMariaData, MYSQL_ASSOC)) {
            $mariaRowArray['client_name'] = $mariaRow['client_name'];
            $mariaRowArray['rpi_id'] = $mariaRow['rpi_id'];
            $mariaRowArray['avr_id'] = $mariaRow['avr_id'];
            $mariaRowArray['avr_data'] = $mariaRow['avr_data'];
            $mariaRowArray['avr_param'] = $mariaRow['avr_param'];
            $mariaRowArray['current_time'] = $mariaRow['current_time'];
                
            array_push($mariaToJson, $mariaRowArray);
        }
            
        echo json_encode($mariaToJson);
        mysql_free_result($getMariaData);
        mysql_close($mariaConnect);
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
            andGetMaria();
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
